//
// Created by moff4 on 28.12.2020.
//

#ifndef GRAPHLAB_TOOLS_H
#define GRAPHLAB_TOOLS_H

inline double i_to_phi(int i, int max, double phi) {
    /**
     * i - 0 .. max
     * return 0 if i == max / 2
     * return -phi if i == 0
     * return phi if i == max
     */
    return (phi / max) * i - (phi / 2);
}

#endif //GRAPHLAB_TOOLS_H
