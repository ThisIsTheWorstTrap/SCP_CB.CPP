#ifndef HANDLES_HPP
#define HANDLES_HPP

#include <cstdint>

namespace Engine
{

    class Handle
    {
        public:
            virtual void set_id(std::int32_t new_id)=0;
            virtual std::int32_t get_id() const=0;
            virtual bool is_valid() const=0;
    };

    class ModelHandle : public Handle
    {
        public:
            ModelHandle(std::int32_t id=-1) : id(id) {};

            void set_id(std::int32_t new_id) override;   
            std::int32_t get_id() const override;
            bool is_valid() const override;


        private:
            std::int32_t id;
    };

    class TextureHandle : public Handle
    {
        public:
            TextureHandle(std::int32_t id=-1) : id(id) {};

            void set_id(std::int32_t new_id) override;        
            std::int32_t get_id() const override;
            bool is_valid() const override;


        private:
            std::int32_t id;
    };

    class SoundHandle : public Handle
    {
        public:
            SoundHandle(std::int32_t id=-1) : id(id) {};

            void set_id(std::int32_t new_id) override;        
            std::int32_t get_id() const override;
            bool is_valid() const override;


        private:
            std::int32_t id;
    };

}

#endif