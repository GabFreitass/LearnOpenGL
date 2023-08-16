#include "utils.h"

char *readFile(const char *filename)
{
    // open file in read mode
    FILE *file = fopen(filename, "rb");
    if (file == NULL)
    {
        printf("ERROR:: cannot open file %s\n", filename);
        return NULL;
    }

    size_t fileSize = getFileSize(file);

    // allocates memory in heap to store file contents.
    char *fileContent = (char *)malloc(fileSize + 1);

    if (fileContent == NULL)
    {
        printf("ERROR:: cannot allocate dynamic memory\n");
        fclose(file);
        return NULL;
    }

    // read the file and store its content in fileContent and returns succesfull bytes read
    size_t bytesRead = fread(fileContent, sizeof(char), fileSize, file);

    if (bytesRead != fileSize)
    {
        printf("ERROR:: cannot read file \"%s\"\n", filename);
        fclose(file);
        free(fileContent);
        return NULL;
    }

    fclose(file);

    // indicate the end of file content string
    fileContent[fileSize] = '\0';

    return fileContent;
}

size_t getFileSize(FILE *file)
{
    // move file indicator to the end of file
    fseek(file, 0, SEEK_END);

    // the position of indicator (end) which is the size of file
    size_t fileSize = ftell(file);

    // move the indicator to the start again
    rewind(file);

    return fileSize;
}

static inline void _destructImage(Image *this)
{
    stbi_image_free(this->data);
}

Image cropImage(int x, int y, int width, int height, const Image *image)
{
    const size_t bytes_per_pixel = sizeof(image->data);
    Image croppedImage = *image;
    croppedImage.width = width;
    croppedImage.height = height;
    for (int row = 0; row < height; row++)
    {
        memcpy(croppedImage.data + row * width * bytes_per_pixel,
               image->data + ((row + y) * image->width + x) * bytes_per_pixel,
               bytes_per_pixel * width);
    }
    return croppedImage;
}

Image initImage(const char *imageFile)
{
    Image image = {0};
    image.name = imageFile;
    image.destructImage = _destructImage;
    image.data = stbi_load(imageFile, &image.width, &image.height, &image.nrChannels, 0);
    if (!image.data)
    {
        printf("Failed to load image \"%s\"\n", imageFile);
    }
    return image;
}