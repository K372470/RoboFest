#include "CameraWebServer.h"
#include <camera-functions/camera_algorithms.h>
#define WSTag "WebServer"

void CameraWebServer::initWifi(const char *ssid, const char *password)
{
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);

  ESP_LOGI(WSTag, "Connecting To Wifi");
  while (WiFi.status() != WL_CONNECTED)
  {
    ESP_LOGI(WSTag, "...");
    delay(1000);
  }
  ESP_LOGI(WSTag, "Wifi Ready!Connect from: %s", WiFi.localIP().toString());
}

esp_err_t CameraWebServer::send_jpeg_handler(httpd_req_t *req)
{
  camera_fb_t *fb = NULL;
  esp_err_t res = ESP_OK;
  size_t fb_len = 0;
  uint8_t *buf = NULL;

  fb = esp_camera_fb_get();
  if (!fb)
  {
    ESP_LOGI(WSTag, "Camera capture failed");
    esp_camera_fb_return(fb);
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }
  res = httpd_resp_set_type(req, "image/jpeg");
  if (res == ESP_OK)
  {
    if (fb->format == PIXFORMAT_JPEG)
    {
      fb_len = fb->len;
      res = httpd_resp_send(req, (const char *)fb->buf, fb->len);
    }
    else
    {
      res = frame2jpg(fb, 80, &buf, &fb_len) ? ESP_OK : ESP_FAIL;
      httpd_resp_send(req, (const char *)buf, fb_len);
    }
  }
  esp_camera_fb_return(fb);
  return res;
}
esp_err_t CameraWebServer::predict_color_handler(httpd_req_t *req)
{
  esp_err_t err;
  lego_color_t color;
  err = getColorOfObject(color);
  if (err == ESP_OK)
    httpd_resp_sendstr(req, String(color).c_str());
  else
    httpd_resp_send_500(req);
  return err;
}
esp_err_t CameraWebServer::get_lines_handler(httpd_req_t *req)
{
  esp_err_t err;
  lego_color_t color;
  err = getFastLine(color);
  if (err == ESP_OK)
    httpd_resp_sendstr(req, String(color).c_str());
  else
    httpd_resp_send_500(req);
  return err;
}
void CameraWebServer::begin()
{
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = 80;

  httpd_uri_t index_uri = {.uri = "/",
                           .method = HTTP_GET,
                           .handler = send_jpeg_handler,
                           .user_ctx = NULL};
  httpd_uri_t predict_color = {.uri = "/color",
                               .method = HTTP_GET,
                               .handler = predict_color_handler,
                               .user_ctx = NULL};
  httpd_uri_t find_lines = {.uri = "/lines",
                            .method = HTTP_GET,
                            .handler = get_lines_handler,
                            .user_ctx = NULL};

  if (httpd_start(&stream_httpd, &config) == ESP_OK)
  {
    httpd_register_uri_handler(stream_httpd, &index_uri);
    httpd_register_uri_handler(stream_httpd, &predict_color);
    httpd_register_uri_handler(stream_httpd, &find_lines);
  }
}
