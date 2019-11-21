#include "nvs_flash.h"
#include "nvs.h"

nvs_handle my_handle;

void persistInit(void)
{
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );  

   // Open
    printf("\n");
    printf("Opening Non-Volatile Storage (NVS) handle... ");

    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        // Read
        printf("Reading restart counter from NVS ... ");
        int32_t restart_counter = 0; // value will default to 0, if not set yet in NVS
        err = nvs_get_i32(my_handle, "restart_counter", &restart_counter);
        switch (err) {
            case ESP_OK:
                printf("Restart counter = %d\n", restart_counter);
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                printf("The value is not initialized yet!\n");
                break;
            default :
                printf("Error (%s) reading!\n", esp_err_to_name(err));
        }

        // Write
        printf("Updating restart counter in NVS ... ");
        restart_counter++;
        err = nvs_set_i32(my_handle, "restart_counter", restart_counter);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

        // Commit written value.
        // After setting any values, nvs_commit() must be called to ensure changes are written
        // to flash storage. Implementations may write to storage at other times,
        // but this is not guaranteed.
        printf("Committing updates in NVS ... ");
        err = nvs_commit(my_handle);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

        // Close
        //nvs_close(my_handle);
    }      
}

void persistSet_i32(const char * key, int32_t value)
{
    esp_err_t err;
    err = nvs_set_i32(my_handle, key, value);
    printf("Set: %s = %d ", key, value);
    printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
    err = nvs_commit(my_handle);
    printf((err != ESP_OK) ? "Commit Failed!\n" : "Commit Done\n");
}

int32_t persistGet_i32(const char * key)
{
    int32_t value;
    esp_err_t err;
    err = nvs_get_i32(my_handle, key, &value);
    switch (err) {
        case ESP_OK:
            printf("Got: %s = %d\n", key, value);
            break;
        case ESP_ERR_NVS_NOT_FOUND:
            printf("%s value is not initialized yet!\n", key);
            value = -1;
            break;
        default :
            printf("Error (%s) reading!\n", esp_err_to_name(err));
            value = -1;
    }
    return value;
}