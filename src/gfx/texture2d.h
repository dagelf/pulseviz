#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include "texture.h"

template<TextureColorFormat T>
class Texture2D : public Texture<T>
{
public:
    Texture2D(size_t width, size_t height);
    void uploadData(const std::vector<float>& data);
    void bind() override;
    void unbind() override;
    size_t getWidth() const;
    size_t getHeight() const;
    void subImage(unsigned int x, unsigned int y, size_t width, size_t height, std::vector<float>& data);
    void setTextureFiltering(TextureFiltering filtering);

protected:
    void setParameters() override;

    size_t width, height;
};

#endif // TEXTURE2D_H
