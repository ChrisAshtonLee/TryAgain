#include <src/UI/UI.h>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <common/data.h>
#include <random>
#include <vector>
#include <src/ui/imgui_impl_glfw.h>
#include <src/imgui_impl_opengl3.h>
#include <imgui/imgui.h>
#include <src/rendering/material.h>
#include <src/programs/sphere.h>
#include<src/TukeyContour.h>
#include <algorithm>
//ImGuiIO& io = ImGui::GetIO();
UI::UI(GLFWwindow* window, const char* glsl_version, UI_DESC desc)
	: io(ImGui::GetIO()), // Initialize ImGuiIO reference  
	m_points(desc.points), // Initialize m_points from UI_DESC  
	m_polygon(desc.polygon), // Initialize m_polygon from UI_DESC  
	m_line(desc.line), // Initialize m_line from UI_DESC  
	m_halfspace(desc.halfspace), // Initialize m_halfspace from UI_DESC  
	m_sphere(desc.sphere) // Initialize m_sphere from UI_DESC  
{
	currentSelections.push_back(Selection{ m_points });
	currentSelections.push_back(Selection{ m_sphere });
	currentSelections.push_back(Selection{ m_polygon });

	hoverSelections.push_back(Selection{ m_points });
	hoverSelections.push_back(Selection{ m_sphere });
	hoverSelections.push_back(Selection{ m_polygon });
	//currentSelections.push_back(Selection{ m_polygon });
	
	
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark(); // Set ImGui style  

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
}
	

UI::~UI()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void UI::NewFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void UI::Render()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UI::DrawDemoWindow(bool* p_open)
{
}

void UI::DrawWindow()
{
	
	static bool placeSphereMode = false;
	Vertex v1 = { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 1.0f) };
	Vertex v2 = { glm::vec3(0.0f, 0.4f, 0.0f), glm::vec3(0.0f, 1.0f, 1.0f) };
	Vertex v3 = { glm::vec3(0.4f, 0.4f, 0.0f), glm::vec3(0.0f, 1.0f, 1.0f) };
	Vertex v4 = { glm::vec3(0.4f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 1.0f) };
	std::vector<Vertex> vertices = { v1, v2, v3, v4 };
	

	ImGui::Begin("Plotting Options");
		if (ImGui::Button("halfspaces")) {
			gui_mode = 0;
		}
		ImGui::SameLine();
		if (ImGui::Button("points")) {
			gui_mode = 1;
		}
		if (ImGui::Button("spheres")) {
			gui_mode = 2;
		}
	
		switch (gui_mode) {
		case 0:
				
				ImGui::Text("Type in hyperplane coefficients:");
				ImGui::InputText("a1", data.input1, IM_ARRAYSIZE(data.input1));
				ImGui::InputText("a2", data.input2, IM_ARRAYSIZE(data.input2));
				ImGui::InputText("a3", data.input3, IM_ARRAYSIZE(data.input3));
				ImGui::ColorEdit3("Color", (float*)&clear_color);

				if (ImGui::Button("Color")) {
					std::cout << "color " << clear_color.w << " " << clear_color.x << " " << clear_color.z << " " << clear_color.z << std::endl;
				}
				ImGui::SameLine();
				if (ImGui::Button("Enter")) {

					m_halfspace->addInstance(std::atof(data.input1), std::atof(data.input2), std::atof(data.input3), glm::vec3(clear_color.x, clear_color.y, clear_color.z));
					m_halfspace->load();
					//halfspace.print();

				}
				if (ImGui::Button("Clear Halfspaces")) {

					m_halfspace->clear();
				}
				break;
			case 1:
				
				ImGui::Text("Point:");
				ImGui::InputText("x y z", data.input1, IM_ARRAYSIZE(data.input1));
				ImGui::ColorEdit3("Color", (float*)&clear_color);
				ImGui::SameLine();
				if (ImGui::Button("Add Point")) {
					glm::vec3 coords = vecFromString(data.input1);

					//if (sizeof(coords)==3){
					//m_points->addPoint(std::atof(data.input1), std::atof(data.input2), std::atof(data.input3), glm::vec3(clear_color.x, clear_color.y, clear_color.z));
					m_points->addInstance(coords.x, coords.y, coords.z, glm::vec3(clear_color.x, clear_color.y, clear_color.z));
					m_points->load();
			

				}
				
				if (click_points)
				{
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.6f, 0.2f, 1.00f));
				}
				else {
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.3f, 0.4f, 1.00f));
				}

				if (ImGui::Button("Place point")) {
					//glm::vec2 screenpos;
					//previewMode = true;
					click_points = !click_points;
					
				}
				ImGui::PopStyleColor();
			
				if (click_points && ImGui::IsKeyDown(ImGuiKey_LeftShift)) {
					previewMode = true;
				
					previewPos = getPreviewPos();
					//previewPos = glm::vec3(0.0f, 0.0f, 0.0f);
					//static float previewDepth = 0.0f; // Start at z=0
					
					if (!previewInstance)
					{
						
						previewInstance = true;
						preview_idx = m_points->getInstanceCount();
						m_points->addInstance(previewPos.x, previewPos.y, previewPos.z, glm::vec3(0.0,1.0,0.0f));
						m_points->load();
					}
					m_points->points[preview_idx].position = previewPos;	
					m_points->load();
					if ( ImGui::IsMouseClicked(0)) {
						m_points->deleteInstance(preview_idx);
						m_points->addInstance(previewPos.x, previewPos.y, previewPos.z, glm::vec3(clear_color.x, clear_color.y, clear_color.z));
						m_points->load();
						//click_points = false;
						previewInstance = false;
				
					
					}
				}
				else {
					previewMode = false;
				}
				if (click_points && previewInstance && !ImGui::IsKeyDown(ImGuiKey_LeftShift)) {
				
					previewInstance = false;
					m_points->deleteInstance(preview_idx);
					m_points->load();
				}
			
				if (ImGui::Button("Select")) {
					select_mode = !select_mode;
				}
				if (select_mode) drawSelectionBox();


				if (ImGui::InputInt("k_level", &k_input)) {

				}
				if (ImGui::Button("TukeyMedian"))
				{
					std::vector<Vertex> selected_points;
					std::vector <glm::vec3> normals;
					// Use selected points if available, otherwise use all points
					if (selection && !currentSelections[GT.POINTS].selectedIndices.empty()) {
						for (int idx : currentSelections[GT.POINTS].selectedIndices) {
							selected_points.push_back(m_points->points[idx]);
						}
					}
					else {
						selected_points = m_points->points;
					}

					if (selected_points.size() >= 3) {
						// --- Step 1: Calculate a representative depth value ---
						float representative_ndc_z = 0.0f;

						// Project the first point to clip space to get its depth in NDC
						glm::mat4 MVP = proj * view * glm::mat4(1.0f);
						glm::vec4 clipSpacePos = MVP * glm::vec4(selected_points[0].position, 1.0f);

						// Convert to NDC by dividing by w; the z-component is our depth
						if (clipSpacePos.w != 0.0f) {
							representative_ndc_z = clipSpacePos.z / clipSpacePos.w;
						}

						// --- FIX: Convert NDC depth [-1, 1] to Window depth [0, 1] ---
						float representative_win_z = (representative_ndc_z + 1.0f) * 0.5f;

						// --- Step 2: Project all selected points to 2D screen space ---
						std::vector<Vertex> screen_space_points;
						for (const auto& pt : selected_points) {
							glm::vec2 screenPos = UIworldToScreen(pt.position, glm::mat4(1.0f), view, proj, scr_width, scr_height);
							// The z-value here is irrelevant for the 2D algorithm
							screen_space_points.push_back(Vertex{ glm::vec3(screenPos.x, screenPos.y, 0.0f), pt.color });
						}

						// --- Step 3: Calculate the 2D contour ---
						TukeyContour TC(screen_space_points, k_input, true);
						int maxdepth = TC.max_depth;
						for (const auto& pair : TC.intersections_with_depth) {
							if (pair.second == maxdepth) {
								std::cout << "Point: (" << pair.first.x << ", " << pair.first.y << ") Depth: " << pair.second << std::endl;
							}
							
						}
						// --- Step 4: Unproject the 2D contour back to 3D using the correct window depth ---
						std::vector <Vertex> projected_contour_points;
						for (const auto& contour_vertex : TC.median_contour) {
							glm::vec2 screenPos = glm::vec2(contour_vertex.position.x, contour_vertex.position.y);
							normals.push_back(contour_vertex.position);
							// Use the correctly converted representative_win_z for unprojection
							projected_contour_points.push_back(Vertex{
								UIscreenToWorld(screenPos, representative_win_z, glm::mat4(1.0f), view, proj, scr_width, scr_height),
								glm::vec3(0.0f, 1.0f, 0.0f) // Color for the contour
								});
						}

						for (const auto& p : projected_contour_points) {
							std::cout << "median point: " << p.position.x << " " << p.position.y << " " << p.position.z << std::endl;
						}
						if (projected_contour_points.size() >= 3) {
							m_polygon->addInstance(projected_contour_points, normals);
							m_polygon->updateInstances();
						}
						else {
							for (auto& p : projected_contour_points)
							{
								m_points->addInstance(p.position.x, p.position.y, p.position.z, glm::vec3(0.0, 1.0, 0.0));
							}
							m_points->load();
						}
					}
					else {
						std::cout << "Not enough points to calculate Tukey Median." << std::endl;
					}
				}



				if (ImGui::Button("Clear Points")) {
					m_polygon->clear();
					m_points->clear();


				}
				ImGui::SameLine();
				if (ImGui::Button("Generate polygon")) {
					std::vector<glm::vec3> normals;
					std::vector<Vertex> points;
					for (int i = 0; i < m_points->points.size(); i++) {
						for (int idx = 0; idx <currentSelections[0].selectedIndices.size(); ++idx) {
							if (currentSelections[0].selectedIndices[idx] == i) {
								normals.push_back( glm::vec3{UIworldToScreen(m_points->points[i].position, glm::mat4(1.0f),view,proj, scr_width,scr_height),0.0f});
								points.push_back(Vertex{ m_points->points[i].position, m_points->original_colors[i] });
							}
						}
					}

					if (points.size() >= 3) {
						m_polygon->addInstance(points, normals);
						m_polygon->updateInstances();
						std::cout << "polygon indices" << m_polygon->indices.size() << std::endl;
						
						
						m_points->unhighlight_selected(currentSelections[0].selectedIndices);
						
						currentSelections[0].selectedIndices.clear(); // Clear selection after polygon creation
					}
					else {
						std::cout << "Not enough points to create a polygon." << std::endl;
					}
				}
				break;
			case 2:
				ImGui::Text("Sphere:");
				ImGui::InputText("x y z", data.input1, IM_ARRAYSIZE(data.input1));
				ImGui::InputText("r", data.input2, IM_ARRAYSIZE(data.input2));
				ImGui::ColorEdit3("Color", (float*)&clear_color);

				if (ImGui::Button("Add Sphere")) {
					glm::vec3 coords = vecFromString(data.input1);
					float size = std::atof(data.input2);
					bool res = m_sphere->addInstance(coords, glm::vec3(size), Material::emerald);

					std::cout << glm::vec3(size).x << " " << glm::vec3(size).y << " " << glm::vec3(size).z << std::endl;

					//m_sphere->load(std::atof(data.input2));
					//m_sphere->updateInstances();
				}
				if (ImGui::Button("Selection Box")) {
					select_mode = !select_mode;
				}
			
				if (select_mode) drawSelectionBox();
				if (ImGui::IsKeyDown(ImGuiKey_LeftShift)) {

					//ImGui::Checkbox("Place Sphere Mode", &placeSphereMode);
					previewMode = true;
				
					//static float previewDepth = 0.0f; // Start at z=0
					static float previewRadius = std::atof(data.input2);
					previewPos = getPreviewPos();
				
					glm::vec3 previewColor(0.2f, 0.8f, 1.0f);

					if (!previewInstance) {
						m_sphere->addInstance(previewPos, glm::vec3(previewRadius), Material::cyan_plastic, previewColor);
						previewInstance = true;
						preview_idx = m_sphere->getInstanceCount()-1;
						//previewShader = Shader(false, "preview.vert", "preview.frag");
					}
					
					m_sphere->offsets[preview_idx] = previewPos;
					m_sphere->updateInstances();
					if (ImGui::IsMouseClicked(0)) {
						m_sphere->deleteInstance(preview_idx);
						m_sphere->addInstance(previewPos, glm::vec3(previewRadius), Material::emerald, glm::vec3(clear_color.x, clear_color.y, clear_color.z));
						//m_sphere->load();
						//click_points = false;
						previewInstance = false;
					}
				
				}
				else {
					
					previewMode = false;
					m_sphere->shaderLoaded = false;
				}
				if (  previewInstance && !ImGui::IsKeyDown(ImGuiKey_LeftShift)) {

					previewInstance = false;
					m_sphere->deleteInstance(preview_idx);
					m_sphere->updateInstances();
				}
				break;
			}
	
		ImGui::End();
		
		
		
}

void UI::DrawAnotherWindow(int opt)
{
	if (opt == 1) {
		ImGui::SetNextWindowPos(ImVec2(100, 50), ImGuiCond_FirstUseEver);
		ImGui::Begin("Inspector");

		ImVec2 mousePos = ImGui::GetMousePos();
		for (int i = 0; i < currentSelections.size(); i++) {
			Selection& sel = currentSelections[i];
			ImGui::Text("Selection %d:", i + 1);
			ImGui::Text("Selected Instances: %d", sel.selectedIndices.size());
			if (sel.selectedIndices.size() > 0) {
				for (int idx : sel.selectedIndices) {
					glm::vec3 worldPos = sel.m_geometryType->getInstanceWorldCoords(idx);
					ImGui::Text("Instance %d: (%.2f, %.2f, %.2f)", idx, worldPos.x, worldPos.y, worldPos.z);
				}
			}
		}
		if (selection){
			ImGui::Text("selection = true");
		}
		else {
			ImGui::Text("selection = false");
		}
		
		//ImGui::Text("Selections: (%.1f , %.1f, %.1f)", mousePos.x, mousePos.y, glm::length(previewDepth));
		ImGui::End();
		data_pointer = ImGui::GetDrawData();
	}
}
glm::vec3 UI::getPreviewPos()
{
	
	ImVec2 mousePos = ImGui::GetMousePos();
	
	previewPos = (mousePos.x / cameraData.scr_width * 2.0f - 1.0f) * cameraData.Right
		- (mousePos.y / cameraData.scr_height * 2.0f - 1.0f) * cameraData.Up;

	previewPos += previewDepth;
	previewPos = glm::vec(previewPos);
	return previewPos;
	
}
glm::vec3 UI::vecFromString(char input[])
{
	std::string str(input);
	std::stringstream ss(str);
	std::vector<float> vec;
	float value;
	while (ss >> value) {
		vec.push_back(value);
	}
	return glm::vec3({ vec[0],vec[1],vec[2] });
}
void UI::drawSelectionBox() {
	
	if (ImGui::IsMouseClicked(0)) {
		startPos = ImGui::GetMousePos();
		//dragging = true;

	}
	if (ImGui::IsMouseDragging(0)) {
		dragging = true;
		currentPos = ImGui::GetMousePos();
		ImDrawList* draw_list = ImGui::GetForegroundDrawList();
		draw_list->AddRect(startPos, currentPos, IM_COL32(0, 255, 0, 255));
	}
	if (dragging && ImGui::IsMouseReleased(0)) {
		endPos = ImGui::GetMousePos();
		dragging = false;
		
		selectObjectsInBox(startPos, endPos);
	
		// Perform selection here
	}
	if (!ImGui::IsMouseDragging(0) && !dragging &&!selection)
	{
		currentPos = ImGui::GetMousePos();
		highlightHoverSelect(currentPos);
	}
	
	if (select_mode && !dragging && selection && ImGui::IsMouseClicked(0) && !ImGui::GetIO().WantCaptureMouse) {
		for (Selection& sel : currentSelections) {
		
			sel.m_geometryType->unhighlight_selected(sel.selectedIndices);
		
			sel.selectedIndices.clear();
		}
		//m_sphere->unhighlight_selected(std::vector<int>(selectedSpheres.begin(), selectedSpheres.end()));
		selection = false;
	}
	if (select_mode && selection && ImGui::IsKeyPressed(ImGuiKey_Delete)) {
		for (auto& sel : currentSelections) {

			// --- FIX: Sort indices in descending (reverse) order before deleting ---
			std::sort(sel.selectedIndices.begin(), sel.selectedIndices.end(), std::greater<int>());

			for (auto& i : sel.selectedIndices) {
				// Now, the highest index is deleted first, keeping other indices valid.
				sel.m_geometryType->deleteInstance(i);
			}

			sel.selectedIndices.clear();
		}

		selection = false;
	}
}
void UI::highlightHoverSelect(ImVec2 p0) {


	// Get the min/max corners of the selection box
	float minX = p0.x - 5.0, maxX = p0.x + 5.0;
	float minY = p0.y - 5.0, maxY = p0.y + 5.0;

	// For each selection, project its position to screen space and check if inside box
	for (int i = 0; i < hoverSelections.size(); i++) {

		Selection& sel = currentSelections[i];
		Selection& sel2 = hoverSelections[i];

		int instances = sel.m_geometryType->getInstanceCount();
		for (int idx = 0; idx < instances; ++idx) {
			glm::vec3 worldPos = sel.m_geometryType->getInstanceWorldCoords(idx);

			glm::vec2 screenPos = UIworldToScreen(worldPos, view, glm::mat4(1.0f), proj, scr_width, scr_height);
			switch (i) {
			case 0: //Points
				if (screenPos.x >= minX && screenPos.x <= maxX && screenPos.y >= minY && screenPos.y <= maxY) {
					
					if (std::find(sel.selectedIndices.begin(), sel.selectedIndices.end(), idx) == sel.selectedIndices.end()) 
					{
						sel.selectedIndices.push_back(idx);
					}
					if (ImGui::IsMouseClicked(0)) 
					{
						sel2.selectedIndices.push_back(idx);
						std::cout << "object hovered selected " << idx << std::endl;
						break;
					}
				}
				else if (sel2.selectedIndices.size() > 0 && ImGui::IsMouseClicked(0))
				{
					sel.m_geometryType->unhighlight_selected(sel.selectedIndices);
					// If the object is hovered but was not previously selected, add it to selection
					sel.selectedIndices.clear();
					sel2.selectedIndices.clear();
					std::cout << "selections cleared" << std::endl;

				}
				else if ((std::find(sel.selectedIndices.begin(), sel.selectedIndices.end(), idx) != sel.selectedIndices.end())
					&& !(std::find(sel2.selectedIndices.begin(), sel2.selectedIndices.end(), idx) != sel2.selectedIndices.end()))
				{
					// If the object is not hovered but was previously selected, remove it from selection
					auto it = std::remove(sel.selectedIndices.begin(), sel.selectedIndices.end(), idx);
					sel.selectedIndices.erase(it, sel.selectedIndices.end());
				
					sel.m_geometryType->unhighlight_selected({ idx });
				}
				break;
			case 1: //Spheres

				glm::vec2 r_c = UIworldToScreen(worldPos + glm::vec3(0.0f, (float)m_sphere->sizes[idx].x, 0.0f), view, glm::mat4(1.0f), proj, scr_width, scr_height); // Assuming uniform size for spheres
				float r = glm::distance(screenPos, r_c);


				if (glm::distance(glm::vec2(p0.x, p0.y), screenPos) <= r) {

					if (std::find(sel.selectedIndices.begin(), sel.selectedIndices.end(), idx) == sel.selectedIndices.end())
					{
						sel.selectedIndices.push_back(idx);
					}

					if (ImGui::IsMouseClicked(0)) {
						sel2.selectedIndices.push_back(idx);
						break; // Exit loop after selecting
					}
				}
				else if (sel2.selectedIndices.size() > 0 && ImGui::IsMouseClicked(0))
				{
					sel.m_geometryType->unhighlight_selected(sel.selectedIndices);
					// If the object is hovered but was not previously selected, add it to selection
					sel.selectedIndices.clear();
					sel2.selectedIndices.clear();
					std::cout << "selections cleared" << std::endl;

				}
				else if ((std::find(sel.selectedIndices.begin(), sel.selectedIndices.end(), idx) != sel.selectedIndices.end())
					&& !(std::find(sel2.selectedIndices.begin(), sel2.selectedIndices.end(), idx) != sel2.selectedIndices.end()))
				{
					// If the object is not hovered but was previously selected, remove it from selection
					auto it = std::remove(sel.selectedIndices.begin(), sel.selectedIndices.end(), idx);
					sel.selectedIndices.erase(it, sel.selectedIndices.end());
					
					sel.m_geometryType->unhighlight_selected({ idx });
				}
				break;
			}
			sel.m_geometryType->highlight_selected(sel.selectedIndices);

		}
	}
}

void UI::selectObjectsInBox( ImVec2 p0, ImVec2 p1 ) {
	
	


	// Get the min/max corners of the selection box
	float minX = std::min(p0.x, p1.x);
	float maxX = std::max(p0.x, p1.x);
	float minY = std::min(p0.y, p1.y);
	float maxY = std::max(p0.y, p1.y);
	
	// For each selection, project its position to screen space and check if inside box
	for (int i = 0; i < currentSelections.size(); i++) {
		
		Selection& sel = currentSelections[i];
		int instances = sel.m_geometryType->getInstanceCount();
		for (int idx = 0; idx < instances; ++idx) {
		
			
			glm::vec3 worldPos = sel.m_geometryType->getInstanceWorldCoords(idx);
			
			
			glm::vec2 screenPos = UIworldToScreen(worldPos, view, glm::mat4(1.0f), proj, scr_width, scr_height);
			
			
			if (screenPos.x >= minX && screenPos.x <= maxX && screenPos.y >= minY && screenPos.y <= maxY) {
				sel.selectedIndices.push_back(idx);
				std::cout << "object selected " << idx << std::endl;
				selection = true;
			}
		}
		
		//m_sphere->highlight_selected(std::vector<int>(selectedSpheres.begin(), selectedSpheres.end()));
		sel.m_geometryType->highlight_selected(sel.selectedIndices);
		std::cout << "selected objects highlighted: " << std::endl;
	
		std::cout<<std::endl;
	}
}

glm::vec2 UI::UIworldToScreen( glm::vec3 worldPos,  glm::mat4 model, glm::mat4 view,  glm::mat4 projection, int screenWidth, int screenHeight) {
	// Model-View-Projection (MVP) matrix
	glm::mat4 MVP = projection * view * model;

	// Transform world coordinates to Normalized Device Coordinates (NDC)
	glm::vec4 clipSpacePos = MVP * glm::vec4(worldPos, 1.0f);

	glm::vec3 ndcPos = glm::vec3(clipSpacePos) / clipSpacePos.w;

	// Convert NDC to screen coordinates
	glm::vec2 screenPos;
	screenPos.x = (ndcPos.x + 1.0f) * 0.5f * screenWidth;
	screenPos.y = (1.0f - ndcPos.y) * 0.5f * screenHeight;  // Note the inverted y-axis

	return screenPos;
}
glm::vec3 UI::UIscreenToWorld(glm::vec2 screenPos, float z, glm::mat4 model, glm::mat4 view, glm::mat4 projection, int screenWidth, int screenHeight)
{
	glm::vec4 ndcPos;
	ndcPos.x = (2.0f * screenPos.x) / screenWidth - 1.0f;
	ndcPos.y = 1.0f - (2.0f * screenPos.y) / screenHeight;
	ndcPos.z = 2.0f * z - 1.0f; // winZ in [0,1], NDC z in [-1,1]
	ndcPos.w = 1.0f;

	glm::mat4 MVP = projection * view * model;
	glm::mat4 inverseMVP = glm::inverse(MVP);

	glm::vec4 worldPos = inverseMVP * ndcPos;
	if (worldPos.w != 0.0f) {
		worldPos /= worldPos.w;
	}
	return glm::vec3(worldPos);
}
void UI::UpdateCameraMatrices(glm::mat4 in_view, glm::mat4 in_proj, int in_scr_width, int in_scr_height)
{
	view = in_view;
	proj = in_proj;
	scr_width = in_scr_width;
	scr_height = in_scr_height;
}



bool UI::sort_descend(int a, int b) {
	return a > b;
}
bool UI::sort_ascend(int a, int b) {
	return a < b;
}