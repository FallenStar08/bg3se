#include <stdafx.h>

#if defined(ENABLE_IMGUI)

#include <Extender/Client/IMGUI/IMGUI.h>
#include <Extender/Client/IMGUI/Objects.h>
#include <CoreLib/Wrappers.h>

#include <Extender/Client/IMGUI/SDL.inl>
#include <Extender/Client/IMGUI/Vulkan.inl>

BEGIN_NS(extui)

template <class T>
T* TreeParent::AddChild()
{
    auto child = Manager->CreateRenderable<T>();
    Children.Add(child->Handle);
    return child;
}

PlatformBackend::~PlatformBackend() {}

RenderingBackend::~RenderingBackend() {}

Renderable::~Renderable() {}


void StyledRenderable::Render()
{
    for (auto const& var : StyleVars) {
        ImGui::PushStyleVar(var.Key, var.Value);
    }

    for (auto const& var : StyleColors) {
        ImGui::PushStyleColor(var.Key, ImVec4(var.Value.r, var.Value.g, var.Value.b, var.Value.a));
    }

    if (SameLine) ImGui::SameLine();

    StyledRender();

    if (!StyleColors.empty()) {
        ImGui::PopStyleColor(StyleColors.size());
    }

    if (!StyleVars.empty()) {
        ImGui::PopStyleVar(StyleVars.size());
    }
}


void StyledRenderable::SetStyleVar(ImGuiStyleVar var, float value)
{
    for (auto& kv : StyleVars) {
        if (kv.Key == var) {
            kv.Value = value;
            return;
        }
    }

    StyleVars.push_back(StyleVar{ var, value });
}


void StyledRenderable::SetStyleColor(ImGuiCol color, glm::vec4 value)
{
    for (auto& kv : StyleColors) {
        if (kv.Key == color) {
            kv.Value = value;
            return;
        }
    }

    StyleColors.push_back(StyleColor{ color, value });
}


void TreeParent::StyledRender()
{
    if (BeginRender()) {
        for (auto childHandle : Children) {
            auto child = Manager->GetRenderable(childHandle);
            if (child) {
                child->Render();
            }
        }
    }

    EndRender();
}


lua::ImguiHandle TreeParent::AddButton(char const* label)
{
    auto btn = AddChild<Button>();
    btn->Label = label;
    return btn;
}


lua::ImguiHandle TreeParent::AddText(char const* label)
{
    auto txt = AddChild<Text>();
    txt->Label = label;
    return txt;
}


lua::ImguiHandle TreeParent::AddSeparatorText(char const* label)
{
    auto txt = AddChild<SeparatorText>();
    txt->Label = label;
    return txt;
}


lua::ImguiHandle TreeParent::AddCheckbox(char const* label, std::optional<bool> checked)
{
    auto e = AddChild<Checkbox>();
    e->Label = label;
    if (checked) e->Checked = *checked;
    return e;
}


lua::ImguiHandle TreeParent::AddRadioButton(char const* label, std::optional<bool> active)
{
    auto e = AddChild<RadioButton>();
    e->Label = label;
    if (active) e->Active = *active;
    return e;
}


lua::ImguiHandle TreeParent::AddInputText(char const* label, std::optional<STDString> value)
{
    auto e = AddChild<InputText>();
    e->Label = label;
    if (value) {
        e->SetText(*value);
    }

    return e;
}


lua::ImguiHandle TreeParent::AddCombo(char const* label)
{
    auto e = AddChild<Combo>();
    e->Label = label;
    return e;
}


lua::ImguiHandle TreeParent::AddDrag(char const* label, std::optional<float> value, std::optional<float> min, std::optional<float> max)
{
    auto e = AddChild<DragScalar>();
    e->Label = label;
    if (value) e->Value = glm::vec4(*value);
    if (min) e->Min = glm::vec4(*min);
    if (max) e->Max = glm::vec4(*max);
    return e;
}

lua::ImguiHandle TreeParent::AddSlider(char const* label, std::optional<float> value, std::optional<float> min, std::optional<float> max)
{
    auto e = AddChild<SliderScalar>();
    e->Label = label;
    if (value) e->Value = glm::vec4(*value);
    if (min) e->Min = glm::vec4(*min);
    if (max) e->Max = glm::vec4(*max);
    return e;
}


lua::ImguiHandle TreeParent::AddInputScalar(char const* label, std::optional<float> value)
{
    auto e = AddChild<InputScalar>();
    e->Label = label;
    if (value) e->Value = glm::vec4(*value);
    return e;
}


lua::ImguiHandle TreeParent::AddColorEdit(char const* label, std::optional<glm::vec3> value)
{
    auto e = AddChild<ColorEdit>();
    e->Label = label;
    if (value) e->Color = glm::vec4(*value, 1.0f);
    return e;
}


lua::ImguiHandle TreeParent::AddColorPicker(char const* label, std::optional<glm::vec3> value)
{
    auto e = AddChild<ColorPicker>();
    e->Label = label;
    if (value) e->Color = glm::vec4(*value, 1.0f);
    return e;
}


bool Window::BeginRender()
{
    return ImGui::Begin(Label.c_str(), Closeable ? &Closed : nullptr, Flags);
}


void Window::EndRender()
{
    ImGui::End();
}


bool Group::BeginRender()
{
    ImGui::BeginGroup();
    return true;
}


void Group::EndRender()
{
    ImGui::EndGroup();
}


bool CollapsingHeader::BeginRender()
{
    return ImGui::CollapsingHeader(Label.c_str());
}

void CollapsingHeader::EndRender()
{}


void Text::StyledRender()
{
    ImGui::TextUnformatted(Label.c_str(), Label.c_str() + Label.size());
}


void SeparatorText::StyledRender()
{
    if (Label.empty()) {
        ImGui::Separator();
    } else {
        ImGui::SeparatorText(Label.c_str());
    }
}


void Button::StyledRender()
{
    if (ImGui::Button(Label.c_str())) {
        // call delegate
    }
}


void Checkbox::StyledRender()
{
    if (ImGui::Checkbox(Label.c_str(), &Checked)) {
        // call delegate
    }
}


void RadioButton::StyledRender()
{
    if (ImGui::RadioButton(Label.c_str(), Active)) {
        // call delegate
    }
}


InputText::InputText()
{
    Text.reserve(256);
}


void InputText::StyledRender()
{
    if (ImGui::InputText(Label.c_str(), Text.data(), Text.capacity())) {
        // call delegate
    }
}

STDString InputText::GetText()
{
    return Text;
}

void InputText::SetText(STDString text)
{
    Text = text;
    Text.reserve(256);
}


void Combo::StyledRender()
{
    char const* preview = nullptr;
    if (SelectedIndex >= 0 && SelectedIndex < (int)Options.size()) {
        preview = Options[SelectedIndex].c_str();
    }

    if (ImGui::BeginCombo(Label.c_str(), preview)) {
        int i = 0;
        for (auto const& val : Options) {
            bool selected = (SelectedIndex == i);
            if (ImGui::Selectable(val.c_str(), &selected)) {
                if (selected) {
                    SelectedIndex = i;
                }
            }

            i++;
        }

        ImGui::EndCombo();
    }
}


void DragScalar::StyledRender()
{
    if (ImGui::DragScalarN(Label.c_str(), ImGuiDataType_Float, &Value, Components, 1.0f, &Min, &Max)) {
        // call delegate
    }
}


void SliderScalar::StyledRender()
{
    if (ImGui::SliderScalarN(Label.c_str(), ImGuiDataType_Float, &Value, Components, &Min, &Max)) {
        // call delegate
    }
}


void InputScalar::StyledRender()
{
    if (ImGui::InputScalarN(Label.c_str(), ImGuiDataType_Float, &Value, Components)) {
        // call delegate
    }
}


void ColorEdit::StyledRender()
{
    if (ImGui::ColorEdit4(Label.c_str(), &Color.x, HasAlpha ? 0 : ImGuiColorEditFlags_NoAlpha)) {
        // call delegate
    }
}


void ColorPicker::StyledRender()
{
    if (ImGui::ColorPicker4(Label.c_str(), &Color.x, HasAlpha ? 0 : ImGuiColorEditFlags_NoAlpha)) {
        // call delegate
    }
}


IMGUIObjectPoolInterface::~IMGUIObjectPoolInterface() {}

IMGUIObjectManager::IMGUIObjectManager()
{
    pools_[(unsigned)IMGUIObjectType::Window] = std::make_unique<IMGUIObjectPool<Window>>();
    pools_[(unsigned)IMGUIObjectType::Group] = std::make_unique<IMGUIObjectPool<Group>>();
    pools_[(unsigned)IMGUIObjectType::CollapsingHeader] = std::make_unique<IMGUIObjectPool<CollapsingHeader>>();
    pools_[(unsigned)IMGUIObjectType::Text] = std::make_unique<IMGUIObjectPool<Text>>();
    pools_[(unsigned)IMGUIObjectType::SeparatorText] = std::make_unique<IMGUIObjectPool<SeparatorText>>();
    pools_[(unsigned)IMGUIObjectType::Button] = std::make_unique<IMGUIObjectPool<Button>>();
    pools_[(unsigned)IMGUIObjectType::Checkbox] = std::make_unique<IMGUIObjectPool<Checkbox>>();
    pools_[(unsigned)IMGUIObjectType::RadioButton] = std::make_unique<IMGUIObjectPool<RadioButton>>();
    pools_[(unsigned)IMGUIObjectType::InputText] = std::make_unique<IMGUIObjectPool<InputText>>();
    pools_[(unsigned)IMGUIObjectType::Combo] = std::make_unique<IMGUIObjectPool<Combo>>();
    pools_[(unsigned)IMGUIObjectType::DragScalar] = std::make_unique<IMGUIObjectPool<DragScalar>>();
    pools_[(unsigned)IMGUIObjectType::SliderScalar] = std::make_unique<IMGUIObjectPool<SliderScalar>>();
    pools_[(unsigned)IMGUIObjectType::InputScalar] = std::make_unique<IMGUIObjectPool<InputScalar>>();
    pools_[(unsigned)IMGUIObjectType::ColorEdit] = std::make_unique<IMGUIObjectPool<ColorEdit>>();
    pools_[(unsigned)IMGUIObjectType::ColorPicker] = std::make_unique<IMGUIObjectPool<ColorPicker>>();
}

Renderable* IMGUIObjectManager::CreateRenderable(IMGUIObjectType type)
{
    auto obj = pools_[(unsigned)type]->Create();
    obj->Manager = this;

    if (type == IMGUIObjectType::Window) {
        windows_.push_back(obj->Handle);
    }

    return obj;
}

Renderable* IMGUIObjectManager::GetRenderable(HandleType handle)
{
    auto type = (handle >> 56);
    if (type > (unsigned)IMGUIObjectType::Max) return nullptr;
    return pools_[type]->Get(handle);
}

bool IMGUIObjectManager::DestroyRenderable(HandleType handle)
{
    auto type = (handle >> 56);
    if (type > (unsigned)IMGUIObjectType::Max) return false;
    return pools_[type]->Destroy(handle);
}

void IMGUIObjectManager::Render()
{
    ImGui::ShowDemoWindow();

    for (auto windowHandle : windows_) {
        auto window = GetRenderable(windowHandle);
        if (window) {
            window->Render();
        }
    }
}


IMGUIManager::IMGUIManager()
{
    platform_ = std::make_unique<SDLBackend>();
    renderer_ = std::make_unique<VulkanBackend>(*this);
}

IMGUIManager::~IMGUIManager()
{
    DestroyUI();
}

void IMGUIManager::EnableHooks()
{
    platform_->EnableHooks();
    renderer_->EnableHooks();
}

void IMGUIManager::DisableHooks()
{
    platform_->DisableHooks();
    renderer_->DisableHooks();
}

void IMGUIManager::InitializeUI()
{
    if (initialized_) return;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    platform_->InitializeUI();
    renderer_->InitializeUI();
    initialized_ = true;
}

void IMGUIManager::DestroyUI()
{
    if (!initialized_) return;

    initialized_ = false;
    renderer_->DestroyUI();
    platform_->DestroyUI();
    ImGui::DestroyContext();
}

void IMGUIManager::EnableUI(bool enabled)
{
    enableUI_ = enabled;
}

void IMGUIManager::SetObjects(IMGUIObjectManager* objects)
{
    objects_ = objects;

    if (objects_ == nullptr) {
        renderer_->ClearFrame();
    }
}

void IMGUIManager::OnRenderBackendInitialized()
{
    InitializeUI();
}

void IMGUIManager::Update()
{
    if (!enableUI_ || !initialized_ || !objects_) return;

    platform_->NewFrame();
    renderer_->NewFrame();
    ImGui::NewFrame();

    objects_->Render();

    ImGui::Render();
    renderer_->FinishFrame();
}

END_NS()

#endif