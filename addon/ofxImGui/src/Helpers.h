#pragma once

#include "ofParameter.h"
#include "ofRectangle.h"
#include "ofTexture.h"
#include "imgui.h"

static const int kGuiMargin = 10;

namespace ofxImGui
{
	struct WindowOpen
	{
		std::stack<std::vector<std::string>> usedNames;
		shared_ptr<ofParameter<bool>> parameter;
		bool value;
	};

	struct Settings
	{
		Settings();

		ofVec2f windowPos;
		ofVec2f windowSize;
		bool mouseOverGui;
		bool windowBlock;
		int treeLevel;

		ofRectangle totalBounds;
		ofRectangle screenBounds;
	};

	static WindowOpen windowOpen;

	const char * GetUniqueName(ofAbstractParameter& parameter);
	const char * GetUniqueName(const std::string& candidate);

	void SetNextWindow(Settings& settings);
	bool BeginWindow(ofParameter<bool>& parameter, Settings& settings, bool collapse = true);
	bool BeginWindow(const string& name, Settings& settings, bool collapse = true, bool * open = nullptr);
	bool BeginWindow(const string& name, Settings& settings, ImGuiWindowFlags flags, bool * open = nullptr);
	void EndWindow(Settings& settings);

	bool BeginTree(ofAbstractParameter& parameter, Settings& settings);
	bool BeginTree(const string& name, Settings& settings);
	void EndTree(Settings& settings);

	void AddGroup(ofParameterGroup& group, Settings& settings);

#if OF_VERSION_MINOR >= 10
	bool AddParameter(ofParameter<glm::ivec2>& parameter);
	bool AddParameter(ofParameter<glm::ivec3>& parameter);
	bool AddParameter(ofParameter<glm::ivec4>& parameter);

	bool AddParameter(ofParameter<glm::vec2>& parameter);
	bool AddParameter(ofParameter<glm::vec3>& parameter);
	bool AddParameter(ofParameter<glm::vec4>& parameter);
#endif

	bool AddParameter(ofParameter<ofVec2f>& parameter);
	bool AddParameter(ofParameter<ofVec3f>& parameter);
	bool AddParameter(ofParameter<ofVec4f>& parameter);

	bool AddParameter(ofParameter<ofFloatColor>& parameter, bool alpha = true);

	template<typename ParameterType>
	bool AddParameter(ofParameter<ParameterType>& parameter);

	bool AddRadio(ofParameter<int>& parameter, vector<string> labels, int columns = 1);
	bool AddStepper(ofParameter<int>& parameter, int step = 1, int stepFast = 100);

	bool AddRange(const string& name, ofParameter<float>& parameterMin, ofParameter<float>& parameterMax, float speed = 0.01f);

#if OF_VERSION_MINOR >= 10
	bool AddValues(const string& name, vector<glm::ivec2>& values, int minValue, int maxValue);
	bool AddValues(const string& name, vector<glm::ivec3>& values, int minValue, int maxValue);
	bool AddValues(const string& name, vector<glm::ivec4>& values, int minValue, int maxValue);

	bool AddValues(const string& name, vector<glm::vec2>& values, float minValue, float maxValue);
	bool AddValues(const string& name, vector<glm::vec3>& values, float minValue, float maxValue);
	bool AddValues(const string& name, vector<glm::vec4>& values, float minValue, float maxValue);
#endif

	bool AddValues(const string& name, vector<ofVec2f>& values, float minValue, float maxValue);
	bool AddValues(const string& name, vector<ofVec3f>& values, float minValue, float maxValue);
	bool AddValues(const string& name, vector<ofVec4f>& values, float minValue, float maxValue);

	template<typename DataType>
	bool AddValues(const string& name, vector<DataType>& values, DataType minValue, DataType maxValue);

	void AddImage(ofBaseHasTexture& hasTexture, const ofVec2f& size);
	void AddImage(ofTexture& texture, const ofVec2f& size);
}

//--------------------------------------------------------------
template<typename ParameterType>
bool ofxImGui::AddParameter(ofParameter<ParameterType>& parameter)
{
	auto tmpRef = parameter.get();
	const auto& info = typeid(ParameterType);
	if (info == typeid(float))
	{
		if (ImGui::SliderFloat(GetUniqueName(parameter), (float *)&tmpRef, parameter.getMin(), parameter.getMax()))
		{
			parameter.set(tmpRef);
			return true;
		}
		return false;
	}
	if (info == typeid(int))
	{
		if (ImGui::SliderInt(GetUniqueName(parameter), (int *)&tmpRef, parameter.getMin(), parameter.getMax()))
		{
			parameter.set(tmpRef);
			return true;
		}
		return false;
	}
	if (info == typeid(bool))
	{
		if (ImGui::Checkbox(GetUniqueName(parameter), (bool *)&tmpRef))
		{
			parameter.set(tmpRef);
			return true;
		}
		return false;
	}

	ofLogWarning(__FUNCTION__) << "Could not create GUI element for type " << info.name();
	return false;
}

//--------------------------------------------------------------
template<typename DataType>
bool ofxImGui::AddValues(const string& name, vector<DataType>& values, DataType minValue, DataType maxValue)
{
	auto result = false;
	const auto& info = typeid(DataType);
	for (int i = 0; i < values.size(); ++i)
	{
		const auto iname = name + " " + ofToString(i);
		if (info == typeid(float))
		{
			result |= ImGui::SliderFloat(GetUniqueName(iname), *values[i], minValue, maxValue);
		}
		else if (info == typeid(int))
		{
			result |= ImGui::SliderInt(GetUniqueName(iname), *values[i], minValue, maxValue);
		}
		else if (info == typeid(bool))
		{
			result |= ImGui::Checkbox(GetUniqueName(iname), *values[i]);
		}
		else
		{
			ofLogWarning("Gui::AddValues") << "Could not create GUI element for type " << info.name();
			return false;
		}
	}
	return result;
}
