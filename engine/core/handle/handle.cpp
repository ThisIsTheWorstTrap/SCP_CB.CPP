#include "handle.hpp"

void ModelHandle::set_id(std::int32_t new_id)
{
    id = new_id;
}

std::int32_t ModelHandle::get_id() const
{
    return id;
}

bool ModelHandle::is_valid() const
{
    return id != -1;
}

void TextureHandle::set_id(std::int32_t new_id)
{
    id = new_id;
}

std::int32_t TextureHandle::get_id() const
{
    return id;
}

bool TextureHandle::is_valid() const
{
    return id != -1;
}

void SoundHandle::set_id(std::int32_t new_id)
{
    id = new_id;
}

std::int32_t SoundHandle::get_id() const
{
    return id;
}

bool SoundHandle::is_valid() const
{
    return id != -1;
}
