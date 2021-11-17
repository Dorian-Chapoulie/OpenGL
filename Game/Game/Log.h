#pragma once
#include <glm/mat3x3.hpp>
void printMatrix(const glm::mat4 mat) {
    printf("-----------------------------------\n");
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            printf("%f\t", mat[row][col]);
        }
        printf("\n");
    }
    printf("-----------------------------------\n");
}