#include "mywifi.h"

#include <ESP8266WiFi.h>
//#include <WiFiClient.h>
#include <ESP8266mDNS.h> // Include the mDNS library

WiFiServer server(80); //Service Port

int findParamEnd(String &req, int start)
{
    int jj = req.indexOf("&", start);
    int kk = req.indexOf(" ", start);
    int end = jj;

    if (jj < 0 || (kk > 0 && kk < jj))
        end = kk;
    return end;
}

int findPathEnd(String &req, int start)
{
    int jj = req.indexOf("?", start);
    int kk = req.indexOf(" ", start);
    int end = jj;

    if (jj < 0 || (kk > 0 && kk < jj))
        end = kk;
    return end;
}

Request::Request(String &req)
{
    this->setup(req);
}

Request::Request()
{
    this->setup("");
}

void Request::setup(String str)
{
    this->req = str;
    int kk = str.indexOf(" ", 0);
    this->type = str.substring(0, kk);
    int jj = findPathEnd(str, kk);
    this->path = str.substring(kk, jj);
}

String Request::getParam(String param)
{
    //  String req=String(reqB);
    int start = this->req.indexOf(param + "=");
    start = this->req.indexOf("=", start);
    if (start < 0)
        return "";
    start++;

    int end = findParamEnd(req, start);

    String val = req.substring(start, end);

    return val;
}

void Request::print()
{
    Serial.print(" req :");
    Serial.print(this->req);
    Serial.println("<");

    Serial.print(" type :");
    Serial.print(this->type);
    Serial.println("<");
    Serial.print(" path :");
    Serial.print(this->path);
    Serial.println("<");
}

Request::operator bool() const
{
    bool result = this->req.length() > 0;
    return result;
};

Request Request::emptyRequest = Request();

MyWifi::MyWifi() {
    this->request = Request();
};


void MyWifi::setupStation(String &network, String &password, String &domain)
{

    delay(500);

    for (int j = 0; j < 20; j++)
    {
        WiFi.begin(network.c_str(), password.c_str());
        Serial.print(" Setup atempting to connect to network ");

        for (int i = 0; i < 10; i++)
        {
            if (WiFi.status() != WL_CONNECTED)
            {
                delay(500);
                Serial.print(".");
            }
            else
            {
                this->state = MyWifi::STATION;

                Serial.println("");
                Serial.println(" WiFi connected");

                if (!MDNS.begin(domain.c_str()))
                { // Start the mDNS responder for esp8266.local
                    Serial.println(" Error setting up MDNS responder!");
                }
                Serial.println(" mDNS responder started");

                // Start the server
                server.begin();
                Serial.println(" Server started");

                // Print the IP address
                Serial.print(" Use this URL to connect: ");
                Serial.print(" http://");
                Serial.print(WiFi.localIP());
                Serial.println("/");
                MDNS.addService("http", "tcp", 80);
                return;
            }
        }
        Serial.println(" Failed to connect to network");

        WiFi.disconnect();
    }
}

void MyWifi::setupStaticStation(int ip1, int ip2, int ip3, int ip4, String &network, String &password)
{

    // Static IP details...
    IPAddress ip(ip1, ip2, ip3, ip4);
    IPAddress gateway(ip1, ip2, ip3, 249);
    IPAddress subnet(255, 255, 255, 0);
    IPAddress dns(ip1, ip2, ip3, 249);

    // Static IP Setup Info Here...
    WiFi.config(ip, dns, gateway, subnet); //If you need Internet Access You should Add DNS also...

    delay(10);

    for (int j = 0; j < 20; j++)
    {

        WiFi.begin(network.c_str(), password.c_str());

        Serial.print(" Setup atempting to connect to network ");

        for (int i = 0; i < 10; i++)
        {
            if (WiFi.status() != WL_CONNECTED)
            {
                delay(500);
                Serial.print(".");
            }
            else
            {
                this->state = MyWifi::STATION;

                Serial.println("");
                Serial.println(" WiFi connected");

                // Start the server
                server.begin();
                Serial.println(" Server started");

                // Print the IP address
                Serial.print(" Use this URL to connect: ");
                Serial.print(" http://");
                Serial.print(WiFi.localIP());
                Serial.println("/");
                return;
            }
        }
        Serial.println(" Failed to connect to network");

        WiFi.disconnect();
    }
}

void MyWifi::setupAP(String &apssid, String &domainAP)
{

    Serial.print("Setting up access point on : ");
    Serial.println(apssid);
    const char *p = const_cast<char *>(apssid.c_str());

    WiFi.softAP(p);

    Serial.println("");
    Serial.println("WiFi connected");

    this->state = MyWifi::AP;
    if (!MDNS.begin(domainAP.c_str()))
    { // Start the mDNS responder for esp8266.local
        Serial.println(" Error setting up MDNS responder!");
    }
    // Start the server
    server.begin();
    Serial.println("Server started");

    Serial.println(" mDNS responder started");

    MDNS.addService("http", "tcp", 80);
    IPAddress myIP = WiFi.softAPIP();

    Serial.print("Device IP address: ");
    Serial.println(myIP);
}


Request MyWifi::getRequest()
{

    this->client = server.available();

    if (!this->client)
    {
        return Request::emptyRequest;
    }

    Serial.println(" Client connected");

    bool avail = 0;
    for (int i = 0; i < 200; i++)
    {
        avail = this->client.available();
        if (avail)
            break;
        delay(1);
    }

    if (avail)
    {
        Serial.print(" avaible: ");
        Serial.println(client.available());

        // Read the first line of the request
        String str = this->client.readStringUntil('\r');
        Serial.print(" requst: ");
        Serial.println(str);

        this->client.flush();

        this->request.setup(str);
        return this->request;
    }
    else
    {

        this->client.stop();
        return Request::emptyRequest;
    }
}

void MyWifi::response(String &mess)
{

    // Return the response
    this->client.println("HTTP/1.1 200 OK");
    this->client.println("Content-Type: text/plain");
    this->client.println("Access-Control-Allow-Origin: *");
    this->client.println(""); //  do not forget this one
    this->client.println(mess);
    this->client.stop();
}

void MyWifi::loop(MyHandler &handler)
{
    Request request = this->getRequest();

    if (request)
    {
        String mess = handler.handleRequest(request);
        this->response(mess);
        delay(1);
    }
}
