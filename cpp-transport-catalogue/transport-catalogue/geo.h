#pragma once

namespace geo {

    struct Coordinates {
        double lat; // Широта
        double lng; // Долгота
        //Оператор равенста для координат
        bool operator==(const Coordinates& other) const {
            return lat == other.lat && lng == other.lng;
        }
        //Оператор не равенства двух координат
        bool operator!=(const Coordinates& other) const {
            return !(*this == other);
        }
    };

    double ComputeDistance(Coordinates from, Coordinates to);

}  // namespace geo