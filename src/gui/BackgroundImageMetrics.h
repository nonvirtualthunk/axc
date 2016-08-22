//
// Created by Sam Bock on 8/15/16.
//

#ifndef TEST2_BACKGROUNDIMAGEMETRICS_H
#define TEST2_BACKGROUNDIMAGEMETRICS_H

#include <glm/vec4.hpp>
#include <graphics/Image.h>
#include <containers/ArxMap.h>

struct BackgroundImageMetrics {
    int borderPixelWidth = 0;
    glm::vec4 centerColor;

    static BackgroundImageMetrics imageMetricsFor(ImagePtr image);
};

BackgroundImageMetrics BackgroundImageMetrics::imageMetricsFor(ImagePtr image) {
    static Arx::Map<Image*, BackgroundImageMetrics> metrics;

    return metrics.getOrElseUpdate(image.get(), [&](){
        BackgroundImageMetrics ret;
        while ( ret.borderPixelWidth < image->width &&
                image->pixel(ret.borderPixelWidth,image->height / 2)[3] > 0 ) {
            ret.borderPixelWidth += 1;
        }
        ret.centerColor = image->pixelV4(image->width - 1, 0);

        return ret;
    });
}

#endif //TEST2_BACKGROUNDIMAGEMETRICS_H
