#include "json_reader.h" 
#include "transport_catalogue.h" 
#include "request_handler.h" 
#include "map_renderer.h" 
#include "transport_router.h"

#include <iostream> 


int main() {

    JsonReader read(std::cin);

    data_transport::TransportCatalogue catalogue;

    read.ApplyRequestBase(catalogue);

    renderer::MapRenderer renderer_catalogue;

    read.ApplyVisualSettingsMap(renderer_catalogue);
  
    data_transport::TransportRouter router = { read.ApplyRouterSetting(), catalogue};

    RequestHandler handler(catalogue, renderer_catalogue, router);
  
    read.ParseRequest(std::cout, handler);

    return 0;

}