#ifndef DUOPLOT_ENUMERATIONS_H_
#define DUOPLOT_ENUMERATIONS_H_

#include <stdint.h>

#include "lumos/plotting/item_id.h"

namespace lumos
{
    namespace internal
    {
        enum class CommunicationHeaderObjectType : uint16_t
        {
            FUNCTION,
            NUM_BUFFERS_REQUIRED,
            NUM_BYTES,
            DATA_STRUCTURE,
            BYTES_PER_ELEMENT,
            DATA_TYPE,
            NUM_CHANNELS,
            NUM_ELEMENTS,
            HAS_COLOR,
            HAS_POINT_SIZES,
            NUM_VERTICES,
            NUM_INDICES,
            NUM_OBJECTS,
            DIMENSION_2D,
            HAS_PAYLOAD,
            AZIMUTH,
            ELEVATION,
            AXIS_MIN_MAX_VEC,
            VEC3,
            SCALE_MATRIX,
            TRANSLATION_VECTOR,
            ROTATION_MATRIX,
            PROJECT_FILE_NAME,
            SCREENSHOT_BASE_PATH,
            TITLE_STRING,
            LABEL,
            HANDLE_STRING,
            INT32,
            POS2D,
            FIGURE_NUM,
            PARENT_NAME,
            PARENT_TYPE,
            ELEMENT_NAME,
            GUI_ELEMENT_TYPE,
            PROPERTY,
            ITEM_ID,
            NUM_NAMES,
            UNKNOWN
        };

        enum class PropertyFlag : uint8_t
        {
            PERSISTENT,
            APPENDABLE,
            INTERPOLATE_COLORMAP,
            UPDATABLE,
            FAST_PLOT,
            EXCLUDE_FROM_SELECTION,
            SELECTABLE,
            UNKNOWN
        };

        enum class PropertyType : uint8_t
        {
            LINE_WIDTH,
            ALPHA,
            Z_OFFSET,
            TRANSFORM,
            NAME,
            DISTANCE_FROM,
            LINE_STYLE,
            COLOR,
            EDGE_COLOR,
            FACE_COLOR,
            SILHOUETTE,
            COLOR_MAP,
            POINT_SIZE,
            BUFFER_SIZE,
            SCATTER_STYLE,
            PROPERTY_FLAG,
            ITEM_ID,
            UNKNOWN
        };

        enum class Function : uint8_t
        {
            DRAW_LINE3D,
            DRAW_ARROW,
            PLANE_XY,
            PLANE_XZ,
            PLANE_YZ,
            GRID_ON,
            GRID_OFF,
            PLOT2,
            PLOT3,
            REAL_TIME_PLOT,
            CUBE,
            SPHERE,
            PROPERTIES_EXTENSION,
            PROPERTIES_EXTENSION_MULTIPLE,
            LINE_COLLECTION2,
            LINE_COLLECTION3,
            STEM,
            FAST_PLOT2,
            FAST_PLOT3,
            PLOT_COLLECTION2,
            PLOT_COLLECTION3,
            SET_CURRENT_ELEMENT,
            CREATE_NEW_ELEMENT,
            DELETE_PLOT_OBJECT,
            CURRENT_ELEMENT_AS_IMAGE_VIEW,
            OPEN_PROJECT_FILE,
            NEW_ELEMENT,
            WAIT_FOR_FLUSH,
            FLUSH_ELEMENT,
            FLUSH_MULTIPLE_ELEMENTS,
            SCATTER2,
            SCATTER3,
            QUIVER,
            QUIVER3,
            DRAW_LINE_BETWEEN_POINTS_3D,
            POLYGON_FROM_4_POINTS,
            DRAW_TRIANGLES_3D,
            DRAW_TRIANGLE_3D,
            DRAW_TILES,
            DRAW_MESH,
            DRAW_MESH_SEPARATE_VECTORS,
            HOLD_ON,
            HOLD_OFF,
            POSITION,
            SURF,
            IM_SHOW,
            DISABLE_AXES_FROM_MIN_MAX,
            SET_AXES_BOX_SCALE_FACTOR,
            SET_TITLE,
            SET_OBJECT_TRANSFORM,
            IS_BUSY_RENDERING,
            GLOBAL_ILLUMINATION,
            AXES_2D,
            AXES_3D,
            VIEW,
            CLEAR,
            SOFT_CLEAR,
            STAIRS,
            SHOW_LEGEND,
            DISABLE_SCALE_ON_ROTATION,
            AXES_SQUARE,
            GET_FLOAT_PARAMETER,
            SCREENSHOT,
            QUERY_FOR_SYNC_OF_GUI_DATA,
            SET_GUI_ELEMENT_LABEL,
            SET_GUI_ELEMENT_ENABLED,
            SET_GUI_ELEMENT_DISABLED,
            SET_GUI_ELEMENT_MIN_VALUE,
            SET_GUI_ELEMENT_MAX_VALUE,
            SET_GUI_ELEMENT_VALUE,
            SET_GUI_ELEMENT_STEP,
            SCREEN_SPACE_PRIMITIVE,
            UNKNOWN
        };

        enum class DataType : uint8_t
        {
            FLOAT,
            DOUBLE,
            INT8,
            INT16,
            INT32,
            INT64,
            UINT8,
            UINT16,
            UINT32,
            UINT64,
            UNKNOWN
        };

        enum class DataStructure : uint8_t
        {
            MATRIX,
            VECTOR,
            IMAGE,
            TEXT,
            UNKNOWN
        };

        enum class ColorT : uint8_t
        {
            RED,
            GREEN,
            BLUE,
            CYAN,
            MAGENTA,
            YELLOW,
            BLACK,
            WHITE,
            GRAY
        };

        enum class FaceColorT : uint8_t
        {
            RED,
            GREEN,
            BLUE,
            CYAN,
            MAGENTA,
            YELLOW,
            BLACK,
            WHITE,
            GRAY,
            NONE
        };

        enum class EdgeColorT : uint8_t
        {
            RED,
            GREEN,
            BLUE,
            CYAN,
            MAGENTA,
            YELLOW,
            BLACK,
            WHITE,
            GRAY,
            NONE
        };

        enum class SilhouetteT : uint8_t
        {
            RED,
            GREEN,
            BLUE,
            CYAN,
            MAGENTA,
            YELLOW,
            BLACK,
            WHITE,
            GRAY
        };

    } // namespace internal

    enum class ElementParent : uint8_t
    {
        TAB,
        WINDOW,
        UNKNOWN
    };

    enum DistanceFromType : uint8_t
    {
        X,
        Y,
        Z,
        XY,
        XZ,
        YZ,
        XYZ
    };

    enum class GuiElementType : uint8_t
    {
        Button,
        Slider,
        Checkbox,
        EditableText,
        DropdownMenu,
        ListBox,
        RadioButtonGroup,
        TextLabel,
        StaticBox,
        PlotPane,
        ScrollingText,
        Unknown
    };

} // namespace lumos

#endif // DUOPLOT_ENUMERATIONS_H_
