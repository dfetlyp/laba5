#include "image.hpp"
#include <iostream>

int main() {
    // Генерация случайного изображения
    const size_t imgWidth = 1000;
    const size_t imgHeight = 1000;
    std::cout << "Generating random image (" << imgWidth << "x" << imgHeight << ")...\n";
    Image img = generateRandomImage(imgWidth, imgHeight);

    std::cout << "Original image:\n";
    displayImageWithColor(img);

    // Последовательное размытие изображения
    Image sequentiallyBlurredImage;
    measureExecutionTime("Sequential blur", [&]() {
        sequentiallyBlurredImage = SequentialBlur(img);
        });
    displayImageWithColor(sequentiallyBlurredImage);

    // Параллельное размытие (с 4 потоками)
    Image parallelBlurredImage;
    measureExecutionTime("Parallel blur (4 threads)", [&]() {
        parallelBlurredImage = ParallelBlur(img, 4);
        });

    // Демонстрация атомарных операций
    std::cout << "\nExample of atomic operations:\n";
    demonstrateAtomicOperations();

    return 0;
}
