#ifndef DUOPLOT_COMMUNICATION_HEADER_OBJECT_H
#define DUOPLOT_COMMUNICATION_HEADER_OBJECT_H

#include <algorithm>
#include <array>
#include <cassert>
#include <climits>
#include <stdexcept>
#include <vector>

#include "lumos/plotting/constants.h"
#include "lumos/plotting/encode_decode_functions_defs.h"
#include "lumos/plotting/enumerations.h"
#include "lumos/plotting/fillable_uint8_array.h"
#include "lumos/logging.h"
#include "lumos/math.h"
#include "lumos/plotting/plot_properties.h"
#include "lumos/plotting/utils.h"

namespace lumos
{
    namespace internal
    {

        struct CommunicationHeaderObject
        {
            CommunicationHeaderObjectType type;
            uint8_t size;
            uint8_t data[kCommunicationHeaderObjectDataSize];

            CommunicationHeaderObject() : type{CommunicationHeaderObjectType::UNKNOWN}, size{0U} {}
            CommunicationHeaderObject(const CommunicationHeaderObjectType input_type) : type{input_type}, size{0U} {}

            template <typename U>
            CommunicationHeaderObject(const CommunicationHeaderObjectType input_type, const U &input_data)
                : type{input_type}, size{sizeof(U)}
            {
                static_assert(std::is_same<U, std::int8_t>::value || std::is_same<U, std::int16_t>::value ||
                              std::is_same<U, std::int32_t>::value || std::is_same<U, std::int64_t>::value ||
                              std::is_same<U, std::uint8_t>::value || std::is_same<U, std::uint16_t>::value ||
                              std::is_same<U, std::uint32_t>::value || std::is_same<U, std::uint64_t>::value ||
                              std::is_same<U, float>::value || std::is_same<U, double>::value);
                static_assert(sizeof(U) <= kCommunicationHeaderObjectDataSize, "Object too big!");
                std::memcpy(data, reinterpret_cast<const uint8_t *const>(&input_data), size);
            }

            CommunicationHeaderObject(const CommunicationHeaderObjectType input_type, const internal::Dimension2D &input_data)
                : type{input_type}, size{2U * sizeof(uint32_t)}
            {
                std::memcpy(data, &(input_data.rows), sizeof(uint32_t));
                std::memcpy(data + sizeof(uint32_t), &(input_data.cols), sizeof(uint32_t));
            }

            CommunicationHeaderObject(const CommunicationHeaderObjectType input_type,
                                      const std::pair<Vec3<double>, Vec3<double>> &input_data)
                : type{input_type}, size{sizeof(double) * 3U * 2U}
            {
                size_t idx = 0U;

                std::memcpy(data + idx, &(input_data.first.x), sizeof(double));
                idx += sizeof(double);

                std::memcpy(data + idx, &(input_data.first.y), sizeof(double));
                idx += sizeof(double);

                std::memcpy(data + idx, &(input_data.first.z), sizeof(double));
                idx += sizeof(double);

                std::memcpy(data + idx, &(input_data.second.x), sizeof(double));
                idx += sizeof(double);

                std::memcpy(data + idx, &(input_data.second.y), sizeof(double));
                idx += sizeof(double);

                std::memcpy(data + idx, &(input_data.second.z), sizeof(double));
                idx += sizeof(double);
            }

            CommunicationHeaderObject(const CommunicationHeaderObjectType input_type, const ItemId &input_data)
                : type{input_type}, size{sizeof(uint16_t)}
            {
                serializeToCommunicationHeaderObject(*this, input_data);
            }

            CommunicationHeaderObject(const CommunicationHeaderObjectType input_type, const internal::DataType &input_data)
                : type{input_type}, size{sizeof(uint8_t)}
            {
                data[0U] = static_cast<uint8_t>(input_data);
            }

            CommunicationHeaderObject(const CommunicationHeaderObjectType input_type, const Vec3<double> &input_data)
                : type{input_type}, size{sizeof(double) * 3U}
            {
                size_t idx = 0U;

                std::memcpy(data + idx, &(input_data.x), sizeof(double));
                idx += sizeof(double);

                std::memcpy(data + idx, &(input_data.y), sizeof(double));
                idx += sizeof(double);

                std::memcpy(data + idx, &(input_data.z), sizeof(double));
                idx += sizeof(double);
            }

            CommunicationHeaderObject(const CommunicationHeaderObjectType input_type,
                                      const FixedSizeMatrix<double, 3, 3> &input_data)
                : type{input_type}, size{sizeof(FixedSizeMatrix<double, 3, 3>)}
            {
                size_t idx = 0U;

                for (size_t r = 0; r < 3; r++)
                {
                    for (size_t c = 0; c < 3; c++)
                    {
                        std::memcpy(data + idx, &(input_data(r, c)), sizeof(double));
                        idx += sizeof(double);
                    }
                }
            }

            CommunicationHeaderObject(const CommunicationHeaderObjectType input_type, const properties::Label &input_data)
                : type{input_type}, size{static_cast<uint8_t>(input_data.length + 1U)}
            {
                serializeToCommunicationHeaderObject(*this, input_data);
            }

            template <typename T>
            T as() const
            {
                static_assert(std::is_same<T, std::int8_t>::value || std::is_same<T, std::int16_t>::value ||
                              std::is_same<T, std::int32_t>::value || std::is_same<T, std::int64_t>::value ||
                              std::is_same<T, std::uint8_t>::value || std::is_same<T, std::uint16_t>::value ||
                              std::is_same<T, std::uint32_t>::value || std::is_same<T, std::uint64_t>::value ||
                              std::is_same<T, float>::value || std::is_same<T, double>::value);

                T out_val;
                std::memcpy(reinterpret_cast<uint8_t *>(&out_val), data, sizeof(T));

                return out_val;
            }
        };

        // Template specializations must be at namespace scope for GCC compatibility.
        // Clang accepts them inside the class but GCC does not (C++ standard requires
        // explicit specializations to be in an enclosing namespace scope).

        template <>
        inline internal::Dimension2D CommunicationHeaderObject::as() const
        {
            internal::Dimension2D dim;
            deserializeFromCommunicationHeaderObject(dim, *this);
            return dim;
        }

        template <>
        inline std::pair<Vec3<double>, Vec3<double>> CommunicationHeaderObject::as() const
        {
            std::pair<Vec3<double>, Vec3<double>> pt;
            deserializeFromCommunicationHeaderObject(pt, *this);
            return pt;
        }

        template <>
        inline internal::DataType CommunicationHeaderObject::as() const
        {
            internal::DataType dt;
            deserializeFromCommunicationHeaderObject(dt, *this);
            return dt;
        }

        template <>
        inline Vec3<double> CommunicationHeaderObject::as() const
        {
            Vec3<double> pt;
            deserializeFromCommunicationHeaderObject(pt, *this);
            return pt;
        }

        template <>
        inline FixedSizeMatrix<double, 3, 3> CommunicationHeaderObject::as() const
        {
            FixedSizeMatrix<double, 3, 3> mat;
            deserializeFromCommunicationHeaderObject(mat, *this);
            return mat;
        }

        template <>
        inline ItemId CommunicationHeaderObject::as() const
        {
            ItemId id;
            deserializeFromCommunicationHeaderObject(id, *this);
            return id;
        }

        // Properties
        template <>
        inline internal::PropertyBase CommunicationHeaderObject::as() const
        {
            internal::PropertyBase pb;
            deserializeFromCommunicationHeaderObject(pb, *this);
            return pb;
        }

        template <>
        inline properties::Label CommunicationHeaderObject::as() const
        {
            properties::Label label;
            deserializeFromCommunicationHeaderObject(label, *this);
            return label;
        }

        template <>
        inline properties::Alpha CommunicationHeaderObject::as() const
        {
            properties::Alpha alpha;
            deserializeFromCommunicationHeaderObject(alpha, *this);
            return alpha;
        }

        template <>
        inline properties::LineWidth CommunicationHeaderObject::as() const
        {
            properties::LineWidth lw;
            deserializeFromCommunicationHeaderObject(lw, *this);
            return lw;
        }

        template <>
        inline internal::ColorInternal CommunicationHeaderObject::as() const
        {
            internal::ColorInternal color;
            deserializeFromCommunicationHeaderObject(color, *this);
            return color;
        }

        template <>
        inline properties::EdgeColor CommunicationHeaderObject::as() const
        {
            properties::EdgeColor color;
            deserializeFromCommunicationHeaderObject(color, *this);
            return color;
        }

        template <>
        inline properties::FaceColor CommunicationHeaderObject::as() const
        {
            properties::FaceColor color;
            deserializeFromCommunicationHeaderObject(color, *this);
            return color;
        }

        template <>
        inline properties::Silhouette CommunicationHeaderObject::as() const
        {
            properties::Silhouette s;
            deserializeFromCommunicationHeaderObject(s, *this);
            return s;
        }

        template <>
        inline properties::ScatterStyle CommunicationHeaderObject::as() const
        {
            properties::ScatterStyle style;
            deserializeFromCommunicationHeaderObject(style, *this);
            return style;
        }

        template <>
        inline properties::ColorMap CommunicationHeaderObject::as() const
        {
            properties::ColorMap cmap;
            deserializeFromCommunicationHeaderObject(cmap, *this);
            return cmap;
        }

        template <>
        inline properties::LineStyle CommunicationHeaderObject::as() const
        {
            properties::LineStyle ls;
            deserializeFromCommunicationHeaderObject(ls, *this);
            return ls;
        }

        template <>
        inline properties::PointSize CommunicationHeaderObject::as() const
        {
            properties::PointSize ps;
            deserializeFromCommunicationHeaderObject(ps, *this);
            return ps;
        }

        template <>
        inline properties::BufferSize CommunicationHeaderObject::as() const
        {
            properties::BufferSize bs;
            deserializeFromCommunicationHeaderObject(bs, *this);
            return bs;
        }

        template <>
        inline properties::DistanceFrom CommunicationHeaderObject::as() const
        {
            properties::DistanceFrom df;
            deserializeFromCommunicationHeaderObject(df, *this);
            return df;
        }

        template <>
        inline properties::ZOffset CommunicationHeaderObject::as() const
        {
            properties::ZOffset zo;
            deserializeFromCommunicationHeaderObject(zo, *this);
            return zo;
        }

        template <>
        inline properties::Transform CommunicationHeaderObject::as() const
        {
            properties::Transform tf;
            deserializeFromCommunicationHeaderObject(tf, *this);
            return tf;
        }

        template <>
        inline PropertyFlag CommunicationHeaderObject::as() const
        {
            PropertyFlag flag;
            deserializeFromCommunicationHeaderObject(flag, *this);
            return flag;
        }

    } // namespace internal
} // namespace lumos

#endif // DUOPLOT_COMMUNICATION_HEADER_OBJECT_H
