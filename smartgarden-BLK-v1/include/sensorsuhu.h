#pragma once
/*********************************
 *  sensorsuhu.cpp
 *********************************/
enum DHT_MODEL_t
{
    DHT11,
    DHT22
};

enum DHT_ERROR_t
{
    ERROR_NONE = 0,
    ERROR_TIMEOUT,
    ERROR_CHECKSUM
};

extern DHT_MODEL_t sensorsuhu_model;
extern DHT_ERROR_t sensorsuhu_error;
void sensorsuhu_read();
