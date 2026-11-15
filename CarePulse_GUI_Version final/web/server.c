
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "structs.h"
#include "member2.h"
#include "member3.h"
#include "member4.h"
#include "../CarePulse_WebServer/api.h"
#pragma comment(lib, "Ws2_32.lib")

#define PORT 8080
#define HTML_FILE "C:\\Users\\HP\\Downloads\\carepulse_hospital_gui.html"
#define REQUEST_SIZE 16384
#define RESPONSE_SIZE 32768

int send_all(SOCKET s, const char* data, int length)
{
    int total = 0;

    while (total < length)
    {
        int sent = send(s, data + total, length - total, 0);
        if (sent <= 0) return 0;
        total += sent;
    }

    return 1;
}

void send_response(SOCKET s, int status, const char* type,
    const char* body)
{
    char header[512];
    const char* statusText = status == 200 ? "OK" :
        status == 201 ? "Created" :
        status == 400 ? "Bad Request" : "Not Found";

    int length = (int)strlen(body);

    snprintf(header, sizeof(header),
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "Cache-Control: no-store\r\n\r\n",
        status, statusText, type, length);

    send_all(s, header, (int)strlen(header));
    send_all(s, body, length);
}

void serve_page(SOCKET s)
{
    printf("Opening dashboard: %s\n", HTML_FILE);
    FILE* file = fopen(HTML_FILE, "rb");

    if (!file)
    {
        send_response(s, 404, "text/plain",
            "Dashboard HTML file not found.");
        return;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char* html = (char*)malloc((size_t)size + 1);

    if (!html)
    {
        fclose(file);
        send_response(s, 400, "text/plain", "Memory error.");
        return;
    }

    size_t readCount = fread(html, 1, (size_t)size, file);
    fclose(file);
    html[readCount] = '\0';

    printf("HTML bytes read: %zu\n", readCount);
    printf("Test comment found: %s\n",
        strstr(html, "CAREPULSE TEST 123") ? "YES" : "NO");

    send_response(s, 200, "text/html; charset=utf-8", html);
    free(html);
}

void handle_client(SOCKET client)
{
    char request[REQUEST_SIZE] = { 0 };
    int received = 0;
    int headerEnd = -1;
    int contentLength = 0;

    while (received < REQUEST_SIZE - 1)
    {
        int n = recv(client, request + received,
            REQUEST_SIZE - 1 - received, 0);

        if (n <= 0) return;

        received += n;
        request[received] = '\0';

        char* end = strstr(request, "\r\n\r\n");

        if (end)
        {
            headerEnd = (int)(end - request) + 4;
            break;
        }
    }

    if (headerEnd < 0)
    {
        send_response(client, 400, "application/json",
            "{\"error\":\"Invalid HTTP request\"}");
        return;
    }

    char* lengthHeader = strstr(request, "Content-Length:");

    if (lengthHeader)
        contentLength = atoi(lengthHeader + 15);

    if (contentLength < 0 ||
        contentLength > REQUEST_SIZE - headerEnd - 1)
    {
        send_response(client, 400, "application/json",
            "{\"error\":\"Request too large\"}");
        return;
    }

    while (received < headerEnd + contentLength)
    {
        int n = recv(client, request + received,
            REQUEST_SIZE - 1 - received, 0);

        if (n <= 0) return;

        received += n;
        request[received] = '\0';
    }

    char method[16] = { 0 };
    char path[256] = { 0 };

    sscanf(request, "%15s %255s", method, path);

    if (strcmp(method, "GET") == 0 &&
        strcmp(path, "/api/patients") == 0)
    {
        char output[RESPONSE_SIZE] = { 0 };

        if (api_get_patients(output, sizeof(output)))
            send_response(client, 200, "application/json", output);
        else
            send_response(client, 400, "application/json",
                "{\"error\":\"Could not read patients\"}");

        return;
    }

    if (strcmp(method, "POST") == 0 &&
        strcmp(path, "/api/patients") == 0)
    {
        char output[1024] = { 0 };
        const char* body = request + headerEnd;

        if (api_add_patient(body, output, sizeof(output)))
            send_response(client, 201, "application/json", output);
        else
            send_response(client, 400, "application/json", output);

        return;
    }

    if (strcmp(method, "GET") == 0 &&
        (strcmp(path, "/") == 0 ||
            strcmp(path, "/index.html") == 0))
    {
        serve_page(client);
        return;
    }

    send_response(client, 404, "application/json",
        "{\"error\":\"Route not found\"}");
}

int main(void)
{
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in serverAddress;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("Winsock initialization failed.\n");
        return 1;
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (serverSocket == INVALID_SOCKET)
    {
        printf("Socket creation failed.\n");
        WSACleanup();
        return 1;
    }

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);

    inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);

    if (bind(serverSocket, (struct sockaddr*)&serverAddress,
        sizeof(serverAddress)) == SOCKET_ERROR)
    {
        printf("Bind failed. Port 8080 may already be in use.\n");
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, 5) == SOCKET_ERROR)
    {
        printf("Listen failed.\n");
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (!initQueue())
    {
        printf("Emergency queue initialization failed.\n");
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (!initializeInventory(INITIAL_MED_CAP))
    {
        printf("Pharmacy initialization failed.\n");
        freeQueue();
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    initAmbulances();
    loadAllHospitalRecords();

    printf("CarePulse Web Server started successfully!\n");
    printf("Open http://127.0.0.1:8080 in your browser.\n");

    while (1)
    {
        clientSocket = accept(serverSocket, NULL, NULL);

        if (clientSocket == INVALID_SOCKET)
            continue;

        handle_client(clientSocket);
        closesocket(clientSocket);
    }

    freeQueue();
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
