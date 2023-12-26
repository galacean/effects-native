//
//  image_loader_ios.cpp
//  PlayGroundIOS
//
//  Created by changxing on 2022/3/8.
//

#include "util/image_loader.h"
#include "player/mars_player.h"
#import "ImageParser.h"

#ifndef PLAYGROUND
#import "MarsEventEmitter.h"
#endif

namespace mn {

//ImageData* ImageLoader::ParseImage(const std::string& path) {
//    NSData* rawData = [NSData dataWithContentsOfFile:[NSString stringWithUTF8String:path.c_str()]];
//    UIImage *image = [[UIImage alloc] initWithData:rawData];
//    vImage_Buffer dest_buffer = {0};
//    [ImageParser parseUIImageToBitmap:&dest_buffer image:image];
//    size_t width = dest_buffer.width;
//    size_t height = dest_buffer.height;
//    image = nil;
//    
//    ImageData* data = new ImageData();
//    data->width = width;
//    data->height = height;
//    data->data = (uint8_t*)dest_buffer.data;
//    
//    return data;
//}

ImageData* ImageLoader::ParseImageData(ImageRawData* raw_data) {
    if (raw_data->decoded) {
        ImageData* data = new ImageData();
        data->width = raw_data->width;
        data->height = raw_data->height;
        data->premultiply_alpha = raw_data->premultiply_alpha;
        data->data = (uint8_t*) malloc(data->width * data->height * 4);
        memcpy(data->data, raw_data->data, data->width * data->height * 4);
        return data;
    }
    NSData *data = [NSData dataWithBytes:(void *)raw_data->data length:raw_data->byte_length];
    UIImage *image = [[UIImage alloc] initWithData:data];
    vImage_Buffer dest_buffer = {0};
    [ImageParser parseUIImageToBitmap:&dest_buffer image:image];
    size_t width = dest_buffer.width;
    size_t height = dest_buffer.height;
    image = nil;
    
    ImageData* image_data = new ImageData();
    image_data->width = width;
    image_data->height = height;
    image_data->premultiply_alpha = true;
    image_data->data = (uint8_t*)dest_buffer.data;
    return image_data;
}

void PlayerEventEmitter::OnEvent(int player_id, int type, std::string msg) {
#ifndef PLAYGROUND
    [MarsEventEmitter onEvent:player_id Type:type Msg:[NSString stringWithUTF8String:msg.c_str()]];
#endif
}


}
