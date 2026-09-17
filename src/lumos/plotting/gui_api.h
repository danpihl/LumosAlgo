#ifndef GUI_API_H
#define GUI_API_H

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "lumos/plotting/enumerations.h"
#include "lumos/plotting/fillable_uint8_array.h"
#include "lumos/plotting/gui_internal.h"
#include "lumos/logging.h"

namespace lumos
{
    namespace gui
    {
        class SliderHandle
        {
        private:
            std::shared_ptr<internal::SliderInternal> internal_ptr_;

            template <typename T>
            friend T getGuiElementHandle(const std::string &handle_string);

            SliderHandle(const std::shared_ptr<internal::InternalGuiElementHandle> &internal_ptr)
                : internal_ptr_{std::dynamic_pointer_cast<internal::SliderInternal>(internal_ptr)}
            {
            }

        public:
            SliderHandle() : internal_ptr_{nullptr} {}

            std::int32_t getMinValue() const
            {
                throwExceptionIfPointerIsNotInitialized(internal_ptr_);
                return internal_ptr_->min_value_;
            }

            std::int32_t getMaxValue() const
            {
                throwExceptionIfPointerIsNotInitialized(internal_ptr_);
                return internal_ptr_->max_value_;
            }

            std::int32_t getStepSize() const
            {
                throwExceptionIfPointerIsNotInitialized(internal_ptr_);
                return internal_ptr_->step_size_;
            }

            std::int32_t getValue() const
            {
                throwExceptionIfPointerIsNotInitialized(internal_ptr_);
                return internal_ptr_->value_;
            }

            double getNormalizedValue() const
            {
                throwExceptionIfPointerIsNotInitialized(internal_ptr_);

                const double val = static_cast<double>(internal_ptr_->value_);
                const double min_val = static_cast<double>(internal_ptr_->min_value_);
                const double max_val = static_cast<double>(internal_ptr_->max_value_);

                return (val - min_val) / (max_val - min_val);
            }

            void setEnabled() const
            {
                throwExceptionIfPointerIsNotInitialized(internal_ptr_);
                internal_ptr_->setEnabled();
            }

            void setDisabled() const
            {
                throwExceptionIfPointerIsNotInitialized(internal_ptr_);
                internal_ptr_->setDisabled();
            }

            void setMinValue(const std::int32_t min_value) const
            {
                throwExceptionIfPointerIsNotInitialized(internal_ptr_);
                internal_ptr_->setMinValue(min_value);
            }

            void setMaxValue(const std::int32_t max_value) const
            {
                throwExceptionIfPointerIsNotInitialized(internal_ptr_);
                internal_ptr_->setMaxValue(max_value);
            }

            void setValue(const std::int32_t value) const
            {
                throwExceptionIfPointerIsNotInitialized(internal_ptr_);
                internal_ptr_->setValue(value);
            }

            void setStepSize(const std::int32_t step_size) const
            {
                throwExceptionIfPointerIsNotInitialized(internal_ptr_);
                internal_ptr_->setStepSize(step_size);
            }
        };

        class ButtonHandle
        {
        private:
            std::shared_ptr<internal::ButtonInternal> internal_ptr_;
            template <typename T>
            friend T getGuiElementHandle(const std::string &handle_string);

            ButtonHandle(const std::shared_ptr<internal::InternalGuiElementHandle> &internal_ptr)
                : internal_ptr_{std::dynamic_pointer_cast<internal::ButtonInternal>(internal_ptr)}
            {
            }

        public:
            ButtonHandle() : internal_ptr_{nullptr} {}

            bool getIsPressed() const
            {
                return internal_ptr_->is_pressed_;
            }

            void setLabel(const std::string &label) const
            {
                internal_ptr_->setLabel(label);
            }
        };

        class CheckboxHandle
        {
        private:
            std::shared_ptr<internal::CheckboxInternal> internal_ptr_;
            template <typename T>
            friend T getGuiElementHandle(const std::string &handle_string);

            CheckboxHandle(const std::shared_ptr<internal::InternalGuiElementHandle> &internal_ptr)
                : internal_ptr_{std::dynamic_pointer_cast<internal::CheckboxInternal>(internal_ptr)}
            {
            }

        public:
            CheckboxHandle() : internal_ptr_{nullptr} {}

            bool getIsChecked() const
            {
                return internal_ptr_->is_checked_;
            }

            void setLabel(const std::string &label) const
            {
                internal_ptr_->setLabel(label);
            }
        };

        class TextLabelHandle
        {
        private:
            std::shared_ptr<internal::TextLabelInternal> internal_ptr_;
            template <typename T>
            friend T getGuiElementHandle(const std::string &handle_string);

            TextLabelHandle(const std::shared_ptr<internal::InternalGuiElementHandle> &internal_ptr)
                : internal_ptr_{std::dynamic_pointer_cast<internal::TextLabelInternal>(internal_ptr)}
            {
            }

        public:
            TextLabelHandle() : internal_ptr_{nullptr} {}

            std::string getLabel() const
            {
                return internal_ptr_->label_;
            }

            void setLabel(const std::string &label) const
            {
                internal_ptr_->setLabel(label);
            }
        };

        class ListBoxHandle
        {
        private:
            std::shared_ptr<internal::ListBoxInternal> internal_ptr_;
            template <typename T>
            friend T getGuiElementHandle(const std::string &handle_string);

            ListBoxHandle(const std::shared_ptr<internal::InternalGuiElementHandle> &internal_ptr)
                : internal_ptr_{std::dynamic_pointer_cast<internal::ListBoxInternal>(internal_ptr)}
            {
            }

        public:
            ListBoxHandle() : internal_ptr_{nullptr} {}

            std::vector<std::string> getElements() const
            {
                return internal_ptr_->elements_;
            }

            std::string getSelectedElement() const
            {
                return internal_ptr_->selected_element_;
            }
        };

        class EditableTextHandle
        {
        private:
            std::shared_ptr<internal::EditableTextInternal> internal_ptr_;
            template <typename T>
            friend T getGuiElementHandle(const std::string &handle_string);

            EditableTextHandle(const std::shared_ptr<internal::InternalGuiElementHandle> &internal_ptr)
                : internal_ptr_{std::dynamic_pointer_cast<internal::EditableTextInternal>(internal_ptr)}
            {
            }

        public:
            EditableTextHandle() : internal_ptr_{nullptr} {}

            std::string getText() const
            {
                return internal_ptr_->text_;
            }

            bool getEnterPressed() const
            {
                return internal_ptr_->enter_pressed_;
            }
        };

        class DropdownMenuHandle
        {
        private:
            std::shared_ptr<internal::DropdownMenuInternal> internal_ptr_;
            template <typename T>
            friend T getGuiElementHandle(const std::string &handle_string);

            DropdownMenuHandle(const std::shared_ptr<internal::InternalGuiElementHandle> &internal_ptr)
                : internal_ptr_{std::dynamic_pointer_cast<internal::DropdownMenuInternal>(internal_ptr)}
            {
            }

        public:
            DropdownMenuHandle() : internal_ptr_{nullptr} {}

            std::vector<std::string> getElements() const
            {
                return internal_ptr_->elements_;
            }

            std::string getSelectedElement() const
            {
                return internal_ptr_->selected_element_;
            }
        };

        class RadioButtonGroupHandle
        {
        private:
            std::shared_ptr<internal::RadioButtonGroupInternal> internal_ptr_;
            template <typename T>
            friend T getGuiElementHandle(const std::string &handle_string);

            RadioButtonGroupHandle(const std::shared_ptr<internal::InternalGuiElementHandle> &internal_ptr)
                : internal_ptr_{std::dynamic_pointer_cast<internal::RadioButtonGroupInternal>(internal_ptr)}
            {
            }

        public:
            RadioButtonGroupHandle() : internal_ptr_{nullptr} {}

            std::vector<std::string> getButtons() const
            {
                return internal_ptr_->getButtons();
            }

            std::int32_t getSelectedButtonIdx() const
            {
                return internal_ptr_->getSelectedButtonIdx();
            }
        };

        using SliderCallbackFunction = std::function<void(const SliderHandle &)>;
        using ButtonCallbackFunction = std::function<void(const ButtonHandle &)>;
        using CheckboxCallbackFunction = std::function<void(const CheckboxHandle &)>;
        using TextLabelCallbackFunction = std::function<void(const TextLabelHandle &)>;
        using ListBoxCallbackFunction = std::function<void(const ListBoxHandle &)>;
        using EditableTextCallbackFunction = std::function<void(const EditableTextHandle &)>;
        using DropdownMenuCallbackFunction = std::function<void(const DropdownMenuHandle &)>;
        using RadioButtonGroupCallbackFunction = std::function<void(const RadioButtonGroupHandle &)>;

    } // namespace gui

    namespace internal
    {

        inline std::map<std::string, gui::SliderCallbackFunction> &getSliderCallbacks()
        {
            static std::map<std::string, gui::SliderCallbackFunction> gui_callbacks;

            return gui_callbacks;
        }

        inline std::map<std::string, gui::ButtonCallbackFunction> &getButtonCallbacks()
        {
            static std::map<std::string, gui::ButtonCallbackFunction> gui_callbacks;

            return gui_callbacks;
        }

        inline std::map<std::string, gui::CheckboxCallbackFunction> &getCheckboxCallbacks()
        {
            static std::map<std::string, gui::CheckboxCallbackFunction> gui_callbacks;

            return gui_callbacks;
        }

        /*inline std::map<std::string, gui::TextLabelCallbackFunction>& getTextLabelCallbacks()
        {
            static std::map<std::string, gui::TextLabelCallbackFunction> gui_callbacks;

            return gui_callbacks;
        }*/

        inline std::map<std::string, gui::ListBoxCallbackFunction> &getListBoxCallbacks()
        {
            static std::map<std::string, gui::ListBoxCallbackFunction> gui_callbacks;

            return gui_callbacks;
        }

        inline std::map<std::string, gui::EditableTextCallbackFunction> &getEditableTextCallbacks()
        {
            static std::map<std::string, gui::EditableTextCallbackFunction> gui_callbacks;

            return gui_callbacks;
        }

        inline std::map<std::string, gui::DropdownMenuCallbackFunction> &getDropdownMenuCallbacks()
        {
            static std::map<std::string, gui::DropdownMenuCallbackFunction> gui_callbacks;

            return gui_callbacks;
        }

        inline std::map<std::string, gui::RadioButtonGroupCallbackFunction> &getRadioButtonGroupCallbacks()
        {
            static std::map<std::string, gui::RadioButtonGroupCallbackFunction> gui_callbacks;

            return gui_callbacks;
        }

        using InternalGuiElementHandleMap = std::map<std::string, std::shared_ptr<internal::InternalGuiElementHandle>>;

    } // namespace internal

    namespace gui
    {

        template <typename Fcb, typename Fg>
        void registerGuiCallback(const std::string &handle_string, const Fcb callback_function, const Fg get_function)
        {
            std::map<std::string, Fcb> &gui_callbacks = get_function();

            if (gui_callbacks.find(handle_string) != gui_callbacks.end())
            {
                LOG_WARNING() << "Gui callback with name " << handle_string
                                      << " already exists! Overwriting old callback...";
            }

            gui_callbacks[handle_string] = callback_function;
        }

        inline void registerGuiCallback(const std::string &handle_string,
                                        std::function<void(const SliderHandle &)> callback_function)
        {
            registerGuiCallback(handle_string, callback_function, internal::getSliderCallbacks);
        }

        inline void registerGuiCallback(const std::string &handle_string,
                                        std::function<void(const ButtonHandle &)> callback_function)
        {
            registerGuiCallback(handle_string, callback_function, internal::getButtonCallbacks);
        }

        inline void registerGuiCallback(const std::string &handle_string,
                                        std::function<void(const CheckboxHandle &)> callback_function)
        {
            registerGuiCallback(handle_string, callback_function, internal::getCheckboxCallbacks);
        }

        /*inline void registerGuiCallback(const std::string& handle_string,
                                        std::function<void(const TextLabelHandle&)> callback_function)
        {
            registerGuiCallback(handle_string, callback_function, internal::getTextLabelCallbacks);
        }*/

        inline void registerGuiCallback(const std::string &handle_string,
                                        std::function<void(const ListBoxHandle &)> callback_function)
        {
            registerGuiCallback(handle_string, callback_function, internal::getListBoxCallbacks);
        }

        inline void registerGuiCallback(const std::string &handle_string,
                                        std::function<void(const EditableTextHandle &)> callback_function)
        {
            registerGuiCallback(handle_string, callback_function, internal::getEditableTextCallbacks);
        }

        inline void registerGuiCallback(const std::string &handle_string,
                                        std::function<void(const DropdownMenuHandle &)> callback_function)
        {
            registerGuiCallback(handle_string, callback_function, internal::getDropdownMenuCallbacks);
        }

        inline void registerGuiCallback(const std::string &handle_string,
                                        std::function<void(const RadioButtonGroupHandle &)> callback_function)
        {
            registerGuiCallback(handle_string, callback_function, internal::getRadioButtonGroupCallbacks);
        }

        template <typename T>
        T getGuiElementHandle(const std::string &handle_string);

        template <>
        inline SliderHandle getGuiElementHandle(const std::string &handle_string)
        {
            internal::InternalGuiElementHandleMap &gui_element_handles = internal::getGuiElementHandles();

            if (gui_element_handles.count(handle_string) == 0U)
            {
                throw std::runtime_error("Gui element with handle string " + handle_string + " does not exist!");
            }

            const std::shared_ptr<internal::InternalGuiElementHandle> gui_element{gui_element_handles[handle_string]};

            if (gui_element->getType() != lumos::GuiElementType::Slider)
            {
                throw std::runtime_error("Gui element with handle string " + handle_string + " is not a slider!");
            }

            return SliderHandle{gui_element};
        }

        template <>
        inline ButtonHandle getGuiElementHandle(const std::string &handle_string)
        {
            internal::InternalGuiElementHandleMap &gui_element_handles = internal::getGuiElementHandles();

            if (gui_element_handles.count(handle_string) == 0U)
            {
                throw std::runtime_error("Gui element with handle string " + handle_string + " does not exist!");
            }

            const std::shared_ptr<internal::InternalGuiElementHandle> gui_element{gui_element_handles[handle_string]};

            if (gui_element->getType() != lumos::GuiElementType::Button)
            {
                throw std::runtime_error("Gui element with handle string " + handle_string + " is not a button!");
            }

            return ButtonHandle{gui_element};
        }

        template <>
        inline CheckboxHandle getGuiElementHandle(const std::string &handle_string)
        {
            internal::InternalGuiElementHandleMap &gui_element_handles = internal::getGuiElementHandles();

            if (gui_element_handles.count(handle_string) == 0U)
            {
                throw std::runtime_error("Gui element with handle string " + handle_string + " does not exist!");
            }

            const std::shared_ptr<internal::InternalGuiElementHandle> gui_element{gui_element_handles[handle_string]};

            if (gui_element->getType() != lumos::GuiElementType::Checkbox)
            {
                throw std::runtime_error("Gui element with handle string " + handle_string + " is not a checkbox!");
            }

            return CheckboxHandle{gui_element};
        }

        template <>
        inline TextLabelHandle getGuiElementHandle(const std::string &handle_string)
        {
            internal::InternalGuiElementHandleMap &gui_element_handles = internal::getGuiElementHandles();

            if (gui_element_handles.count(handle_string) == 0U)
            {
                throw std::runtime_error("Gui element with handle string " + handle_string + " does not exist!");
            }

            const std::shared_ptr<internal::InternalGuiElementHandle> gui_element{gui_element_handles[handle_string]};

            if (gui_element->getType() != lumos::GuiElementType::TextLabel)
            {
                throw std::runtime_error("Gui element with handle string " + handle_string + " is not a text label!");
            }

            return TextLabelHandle{gui_element};
        }

        template <>
        inline ListBoxHandle getGuiElementHandle(const std::string &handle_string)
        {
            internal::InternalGuiElementHandleMap &gui_element_handles = internal::getGuiElementHandles();

            if (gui_element_handles.count(handle_string) == 0U)
            {
                throw std::runtime_error("Gui element with handle string " + handle_string + " does not exist!");
            }

            const std::shared_ptr<internal::InternalGuiElementHandle> gui_element{gui_element_handles[handle_string]};

            if (gui_element->getType() != lumos::GuiElementType::ListBox)
            {
                throw std::runtime_error("Gui element with handle string " + handle_string + " is not a list box!");
            }

            return ListBoxHandle{gui_element};
        }

        template <>
        inline EditableTextHandle getGuiElementHandle(const std::string &handle_string)
        {
            internal::InternalGuiElementHandleMap &gui_element_handles = internal::getGuiElementHandles();

            if (gui_element_handles.count(handle_string) == 0U)
            {
                throw std::runtime_error("Gui element with handle string " + handle_string + " does not exist!");
            }

            const std::shared_ptr<internal::InternalGuiElementHandle> gui_element{gui_element_handles[handle_string]};

            if (gui_element->getType() != lumos::GuiElementType::EditableText)
            {
                throw std::runtime_error("Gui element with handle string " + handle_string + " is not an editable text!");
            }

            // TODO: Solve this in a better way.
            // Since enter pressed is a one-time event, we need to reset it here in the get function
            // when the user gets the state in a synchronous way (from user application code, not from the callback function)
            // std::dynamic_pointer_cast<internal::EditableTextInternal>(gui_element)->setEnterPressed(false);

            return EditableTextHandle{gui_element};
        }

        template <>
        inline DropdownMenuHandle getGuiElementHandle(const std::string &handle_string)
        {
            internal::InternalGuiElementHandleMap &gui_element_handles = internal::getGuiElementHandles();

            if (gui_element_handles.count(handle_string) == 0U)
            {
                throw std::runtime_error("Gui element with handle string " + handle_string + " does not exist!");
            }

            const std::shared_ptr<internal::InternalGuiElementHandle> gui_element{gui_element_handles[handle_string]};

            if (gui_element->getType() != lumos::GuiElementType::DropdownMenu)
            {
                throw std::runtime_error("Gui element with handle string " + handle_string + " is not a drop down menu!");
            }

            return DropdownMenuHandle{gui_element};
        }

        template <>
        inline RadioButtonGroupHandle getGuiElementHandle(const std::string &handle_string)
        {
            internal::InternalGuiElementHandleMap &gui_element_handles = internal::getGuiElementHandles();

            if (gui_element_handles.count(handle_string) == 0U)
            {
                throw std::runtime_error("Gui element with handle string " + handle_string + " does not exist!");
            }

            const std::shared_ptr<internal::InternalGuiElementHandle> gui_element{gui_element_handles[handle_string]};

            if (gui_element->getType() != lumos::GuiElementType::RadioButtonGroup)
            {
                throw std::runtime_error("Gui element with handle string " + handle_string + " is not a radio button group!");
            }

            return RadioButtonGroupHandle{gui_element};
        }

    } // namespace gui

    namespace internal
    {

        inline void callGuiCallbackFunction(const ReceivedGuiData &received_gui_data)
        {
            size_t idx = 0U;

            const std::uint8_t *const raw_data = received_gui_data.data();

            const lumos::GuiElementType type{raw_data[idx]};
            idx += sizeof(std::uint8_t);

            const std::uint8_t handle_string_length = raw_data[1];
            idx += sizeof(std::uint8_t);

            std::string handle_string = "";

            for (std::size_t k = 0; k < handle_string_length; k++)
            {
                handle_string.push_back(raw_data[idx]);
                idx += sizeof(std::uint8_t);
            }

            std::uint32_t payload_size;
            std::memcpy(&payload_size, raw_data + idx, sizeof(std::uint32_t));
            idx += sizeof(std::uint32_t);

            UInt8ArrayView payload_data_view{raw_data + idx, payload_size};

            if (type == lumos::GuiElementType::Slider)
            {
                std::map<std::string, gui::SliderCallbackFunction> &gui_callbacks = getSliderCallbacks();

                if (gui_callbacks.find(handle_string) != gui_callbacks.end())
                {
                    gui_callbacks[handle_string](gui::getGuiElementHandle<gui::SliderHandle>(handle_string));
                }
            }
            else if (type == lumos::GuiElementType::Button)
            {
                std::map<std::string, gui::ButtonCallbackFunction> &gui_callbacks = getButtonCallbacks();

                if (gui_callbacks.find(handle_string) != gui_callbacks.end())
                {
                    gui_callbacks[handle_string](gui::getGuiElementHandle<gui::ButtonHandle>(handle_string));
                }
            }
            else if (type == lumos::GuiElementType::Checkbox)
            {
                std::map<std::string, gui::CheckboxCallbackFunction> &gui_callbacks = getCheckboxCallbacks();

                if (gui_callbacks.find(handle_string) != gui_callbacks.end())
                {
                    gui_callbacks[handle_string](gui::getGuiElementHandle<gui::CheckboxHandle>(handle_string));
                }
            }
            /*else if (type == lumos::GuiElementType::TextLabel)
            {
                std::map<std::string, gui::TextLabelCallbackFunction>& gui_callbacks = getTextLabelCallbacks();

                if (gui_callbacks.find(handle_string) != gui_callbacks.end())
                {
                    gui_callbacks[handle_string](gui::getGuiElementHandle<gui::TextLabelHandle>(handle_string));
                }
            }*/
            else if (type == lumos::GuiElementType::ListBox)
            {
                std::map<std::string, gui::ListBoxCallbackFunction> &gui_callbacks = getListBoxCallbacks();

                if (gui_callbacks.find(handle_string) != gui_callbacks.end())
                {
                    gui_callbacks[handle_string](gui::getGuiElementHandle<gui::ListBoxHandle>(handle_string));
                }
            }
            else if (type == lumos::GuiElementType::EditableText)
            {
                std::map<std::string, gui::EditableTextCallbackFunction> &gui_callbacks = getEditableTextCallbacks();

                if (gui_callbacks.find(handle_string) != gui_callbacks.end())
                {
                    gui_callbacks[handle_string](gui::getGuiElementHandle<gui::EditableTextHandle>(handle_string));
                }
            }
            else if (type == lumos::GuiElementType::DropdownMenu)
            {
                std::map<std::string, gui::DropdownMenuCallbackFunction> &gui_callbacks = getDropdownMenuCallbacks();

                if (gui_callbacks.find(handle_string) != gui_callbacks.end())
                {
                    gui_callbacks[handle_string](gui::getGuiElementHandle<gui::DropdownMenuHandle>(handle_string));
                }
            }
            else if (type == lumos::GuiElementType::RadioButtonGroup)
            {
                std::map<std::string, gui::RadioButtonGroupCallbackFunction> &gui_callbacks = getRadioButtonGroupCallbacks();

                if (gui_callbacks.find(handle_string) != gui_callbacks.end())
                {
                    gui_callbacks[handle_string](gui::getGuiElementHandle<gui::RadioButtonGroupHandle>(handle_string));
                }
            }
        }

    } // namespace internal

    namespace gui
    {
        inline void startGuiReceiveThread()
        {
            internal::initTcpSocket();

            if (internal::isDuoplotRunning())
            {
                std::thread query_thread([]()
                                         {
            // Sleep 100ms in order for client execution to create the waiting
            // TCP connection that receives the data GUI from the duoplot application
            usleep(1000U * 100U);
            internal::queryForSyncOfGuiData(); });
                query_thread.detach();
            }

            internal::waitForSyncForAllGuiElements();

            std::thread gui_receive_thread([]()
                                           {
        while (true)
        {
            // TODO: Fix case where client app has already been updated once,
            // and then duoplot restarts and tries to submit gui data again.
            // Preferebly that should just be handled by this loop, as
            // any normal gui element update.

            // receiveGuiData is a blocking method
            const internal::ReceivedGuiData received_data{internal::receiveGuiData()};

            // updateGuiState must execute before callGuiCallbackFunction to properly
            // update the gui element state before calling the callback function.
            internal::updateGuiState(received_data);
            internal::callGuiCallbackFunction(received_data);
        } });

            gui_receive_thread.detach();

            std::thread duoplot_application_heart_beat_monitor_thread([]()
                                                                      {
        while (true)
        {
            // TODO: If duoplot is not running, kill gui_receive_thread and call waitForSyncForAllGuiElements again
            usleep(1000U * 1000U);
            if (internal::isDuoplotRunning())
            {
                // LOG_INFO() << "duoplot is running!";
            }
            else
            {
                // LOG_ERROR() << "duoplot is not running!";
            }
        } });

            duoplot_application_heart_beat_monitor_thread.detach();
        }

    } // namespace gui

} // namespace lumos

#endif // GUI_API_H
