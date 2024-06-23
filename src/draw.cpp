#include "draw.hpp"

void draw::menu() {
    ImGui::Begin("crosshair | github.com/prrefer/crosshair");
    ImGui::Checkbox("crosshair", &options::crosshair);
    ImGui::SameLine();
    ImGui::ColorEdit3("", options::color, 0);
    ImGui::Checkbox("t", &options::t);
    ImGui::SliderFloat("seperation", &options::seperation, 0, 10);
    ImGui::SliderFloat("width", &options::width, 1, 5);
    ImGui::SliderFloat("size", &options::size, 1, 20);
    ImGui::Checkbox("streamproof", &options::streamproof);
    ImGui::End();
}

void draw::line(float x1, float y1, float x2, float y2, float col[3], float width) {
	ImGui::GetBackgroundDrawList()->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), ImGui::ColorConvertFloat4ToU32(ImVec4(col[0], col[1], col[2], 1.0)), width);
}