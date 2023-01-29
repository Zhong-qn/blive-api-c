#include <stdio.h>

#include <blive_api/blive_api.h>

int main()
{
    blive*      entity = NULL;

    blive_api_init();
    blive_create(&entity, 0, 25348832);
    blive_establish_connection(entity, NULL, NULL);
    blive_perform(entity, 20);
    blive_close_connection(entity);
    blive_destroy(entity);
    blive_api_deinit();
    return 0;
}