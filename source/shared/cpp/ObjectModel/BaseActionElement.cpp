// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
#include "pch.h"
#include "BaseActionElement.h"
#include "BaseElement.h"
#include "ParseUtil.h"
#include "Icon.h"

using namespace AdaptiveCards;

constexpr const char* const BaseActionElement::defaultStyle;

BaseActionElement::BaseActionElement(ActionType type) :
    m_style(BaseActionElement::defaultStyle), m_isEnabled(true), m_type(type), m_mode(Mode::Primary),
    m_role(type == ActionType::OpenUrl ? ActionRole::Link : ActionRole::Button)
{
    SetTypeString(ActionTypeToString(type));
    PopulateKnownPropertiesSet();
}

const std::string& BaseActionElement::GetTitle() const
{
    return m_title;
}

void BaseActionElement::SetTitle(std::string&& value)
{
    m_title = std::move(value);
}

void BaseActionElement::SetTitle(const std::string& value)
{
    m_title = value;
}

const std::string& BaseActionElement::GetIconUrl(const ACTheme theme) const {
    return ThemedUrl::GetThemedUrl(theme, m_themedIconUrls, m_iconUrl);
}

const std::string& BaseActionElement::GetIconUrl() const
{
    return m_iconUrl;
}

std::string BaseActionElement::GetSVGPath() const
{
    return GetSVGPath(m_iconUrl);
}

std::string BaseActionElement::GetSVGPath(const std::string& iconUrl) const
{
    std::regex regex{R"([,:]+)"}; // split on ':' and ','
    std::sregex_token_iterator it{iconUrl.begin(), iconUrl.end(), regex, -1};
    std::vector<std::string> config{it, {}};
    std::string iconStyle = IconStyleToString(IconStyle::Regular);
    std::string iconName = "";
    if(config.size() >= 2) {
        iconName = config[1];
        if(config.size() > 2) {
            iconStyle = config[config.size() - 1];
        }
    }
    std::string m_url = iconName + "/" + iconName + ".json";
    return m_url;
}

void BaseActionElement::SetIconUrl(std::string&& value)
{
    m_iconUrl = std::move(value);
}

void BaseActionElement::SetIconUrl(const std::string& value)
{
    m_iconUrl = value;
}

const std::string& BaseActionElement::GetStyle() const
{
    return m_style;
}

void BaseActionElement::SetStyle(std::string&& value)
{
    m_style = std::move(value);
}

void BaseActionElement::SetStyle(const std::string& value)
{
    m_style = value;
}

const std::string& BaseActionElement::GetTooltip() const
{
    return m_tooltip;
}

void BaseActionElement::SetTooltip(std::string&& value)
{
    m_tooltip = std::move(value);
}

void BaseActionElement::SetTooltip(const std::string& value)
{
    m_tooltip = value;
}

void BaseActionElement::SetMode(const Mode value)
{
    m_mode = value;
}

bool BaseActionElement::GetIsEnabled() const
{
    return m_isEnabled;
}

void AdaptiveCards::BaseActionElement::SetIsEnabled(const bool isEnabled)
{
    m_isEnabled = isEnabled;
}

bool BaseActionElement::GetIsRtl() const
{
    return m_isRtl;
}

void AdaptiveCards::BaseActionElement::SetIsRtl(const bool isRtl)
{
    m_isRtl = isRtl;
}

ActionType BaseActionElement::GetElementType() const
{
    return m_type;
}

Mode BaseActionElement::GetMode() const
{
    return m_mode;
}

ActionRole BaseActionElement::GetRole() const
{
    return m_role;
}

void AdaptiveCards::BaseActionElement::SetRole(const ActionRole role)
{
    m_role = role;
}

Json::Value BaseActionElement::SerializeToJsonValue() const
{
    Json::Value root = BaseElement::SerializeToJsonValue();

    if (!m_iconUrl.empty())
    {
        root[AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::IconUrl)] = m_iconUrl;
    }

    if (!m_title.empty())
    {
        root[AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::Title)] = m_title;
    }

    if (!m_style.empty() && (m_style.compare(defaultStyle) != 0))
    {
        root[AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::Style)] = m_style;
    }
    if (m_mode != Mode::Primary)
    {
        root[AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::Mode)] = ModeToString(m_mode);
    }

    if (!m_tooltip.empty())
    {
        root[AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::Tooltip)] = m_tooltip;
    }

    if (!m_isEnabled)
    {
        root[AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::IsEnabled)] = m_isEnabled;
    }

    if (m_role != ActionRole::Button)
    {
        root[AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::ActionRole)] = ActionRoleToString(m_role);
    }

    for (const auto& menuActions : m_menuActions)
    {
        root[AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::MenuActions)].append(menuActions->SerializeToJsonValue());
    }

    return root;
}

void BaseActionElement::PopulateKnownPropertiesSet()
{
    m_knownProperties.insert(
        {AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::IconUrl),
         AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::Style),
         AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::Title),
         AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::Mode),
         AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::Tooltip),
         AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::IsEnabled),
         AdaptiveCardSchemaKeyToString(AdaptiveCardSchemaKey::ActionRole)});
}

void BaseActionElement::GetResourceInformation(std::vector<RemoteResourceInformation>& resourceInfo)
{
    if (!m_iconUrl.empty())
    {
        RemoteResourceInformation imageResourceInfo;
        imageResourceInfo.url = m_iconUrl;
        imageResourceInfo.mimeType = "image";
        resourceInfo.push_back(imageResourceInfo);
    }
}

const std::vector<std::shared_ptr<AdaptiveCards::BaseActionElement>>& BaseActionElement::GetMenuActions() const
{
    return m_menuActions;
}

void BaseActionElement::ParseJsonObject(AdaptiveCards::ParseContext& context, const Json::Value& json, std::shared_ptr<BaseElement>& baseElement)
{
    baseElement = ParseUtil::GetActionFromJsonValue(context, json);
}

std::shared_ptr<BaseActionElement> BaseActionElement::DeserializeBasePropertiesFromString(ParseContext& context, const std::string& jsonString)
{
    return BaseActionElement::DeserializeBaseProperties(context, ParseUtil::GetJsonValueFromString(jsonString));
}

std::shared_ptr<BaseActionElement> BaseActionElement::DeserializeBaseProperties(ParseContext& context, const Json::Value& json)
{
    std::shared_ptr<BaseActionElement> baseActionElement = std::make_shared<BaseActionElement>();
    DeserializeBaseProperties(context, json, baseActionElement);
    return baseActionElement;
}

void BaseActionElement::DeserializeBaseProperties(ParseContext& context, const Json::Value& json, std::shared_ptr<BaseActionElement>& element)
{
    ParseUtil::ThrowIfNotJsonObject(json);

    element->DeserializeBase<BaseActionElement>(context, json);
    element->SetTitle(ParseUtil::GetString(json, AdaptiveCardSchemaKey::Title));
    element->SetIconUrl(ParseUtil::GetString(json, AdaptiveCardSchemaKey::IconUrl));
    element->SetStyle(ParseUtil::GetString(json, AdaptiveCardSchemaKey::Style, defaultStyle, false));
    element->SetMode(ParseUtil::GetEnumValue<Mode>(json, AdaptiveCardSchemaKey::Mode, Mode::Primary, ModeFromString));
    element->SetTooltip(ParseUtil::GetString(json, AdaptiveCardSchemaKey::Tooltip));
    element->SetIsEnabled(ParseUtil::GetBool(json, AdaptiveCardSchemaKey::IsEnabled, true));
    element->SetRole(ParseUtil::GetEnumValue<ActionRole>(json, AdaptiveCardSchemaKey::ActionRole, ActionRole::Button, ActionRoleFromString));
    element->SetIsRtl(ParseUtil::GetBool(json, AdaptiveCardSchemaKey::Rtl, false));

    auto themedUrls = ParseUtil::GetElementCollectionOfSingleType<ThemedUrl>(context, json, AdaptiveCardSchemaKey::ThemedIconUrls, ThemedUrl::Deserialize, false);
    element->m_themedIconUrls = std::move(themedUrls);

    if (IsSplitActionSupported(element->GetElementType())) {
        //Parse Menu Actions
        auto menuActions = ParseUtil::GetElementCollection<BaseActionElement>(
                true,
                context,
                json,
                AdaptiveCardSchemaKey::MenuActions,
                false);

        // Filter the collection to include only allowed items into menuActions
        std::vector<std::shared_ptr<BaseActionElement>> filteredMenuActions;
        for (const auto& action : menuActions)
        {
            if (IsValidMenuAction(action->GetElementType())) {
                filteredMenuActions.push_back(action);
            }
        }
        element->m_menuActions = std::move(filteredMenuActions);

        if (!element->m_menuActions.empty()) {
            // Set the mode, override mode to primary if menuActions are present
            element->SetMode(Mode::Primary);

            // Avoid nesting of menuActions within menuActions by setting such instances to empty
            for (const auto& menuAction: element->m_menuActions) {
                if (!menuAction->m_menuActions.empty()) {
                    menuAction->m_menuActions.clear();
                }
            }
        }
    }
}

bool BaseActionElement::GetIsSplitAction() const
{
    return !m_menuActions.empty();
}
