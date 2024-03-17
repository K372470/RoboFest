#include "WiFi.h"
#include <esp_camera.h>
#include <esp_http_server.h>

class CameraWebServer
{
private:
  static esp_err_t send_jpeg_handler(httpd_req_t *req);
  static esp_err_t predict_color_handler(httpd_req_t *req);
  static esp_err_t get_lines_handler(httpd_req_t *req);
  httpd_handle_t stream_httpd = NULL;

public:
  void initWifi(const char *ssid, const char *password);

  void begin();
};
