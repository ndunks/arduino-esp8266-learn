#include "web.h"


void handle_set_password(String &response, HTTPMethod method)
{

    if (server.hasArg(F("password")))
    {
        String password = server.arg(F("password"));
        if (password.length() > 8)
        {
            response += F("Password terlalu panjang, maks 8 huruf");
        }
        else
        {
            strncpy(config->password, password.c_str(), password.length());
            // make sure null terminated
            config->password[password.length()] = 0;
            if (config_save())
            {
                response += F("OK, Berhasil disimpan");
            }
            else
            {
                response += F("Gagal menyimpan");
            }
        }
    }
    else
    {
        response += F("Password tidak boleh kosong");
    }
}
