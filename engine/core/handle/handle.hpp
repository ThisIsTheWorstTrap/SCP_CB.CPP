#ifndef HANDLES_HPP
#define HANDLES_HPP

#include <cstdint>

class Handle
{
    public:
        virtual std::int32_t get_id() const=0;
        virtual bool is_valid() const=0;
};

class ModelHandle : public Handle
{
    public:
        void set_id(std::int32_t new_id);        
        std::int32_t get_id() const;
        bool is_valid() const;


    private:
        std::int32_t id;
};

class TextureHandle : public Handle
{
    public:
        void set_id(std::int32_t new_id);        
        std::int32_t get_id() const;
        bool is_valid() const;


    private:
        std::int32_t id;
};

class SoundHandle : public Handle
{
    public:
        void set_id(std::int32_t new_id);        
        std::int32_t get_id() const;
        bool is_valid() const;


    private:
        std::int32_t id;
};

#endif