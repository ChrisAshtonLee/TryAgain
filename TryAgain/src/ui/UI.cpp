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
#include <src/geometry_primitive/sphere.h>
#include<src/TukeyContour.h>
#include<scripts/sym_projection.h>
#include<math.h>
#include<fstream>
#include <algorithm>

//ImGuiIO& io = ImGui::GetIO();
UI::UI(GLFWwindow* window, const char* glsl_version, UI_DESC desc)
	: io(ImGui::GetIO()), // Initialize ImGuiIO reference  
	m_points(desc.points), // Initialize m_points from UI_DESC  
	m_polygon(desc.polygon), // Initialize m_polygon from UI_DESC  
	m_line(desc.line), // Initialize m_line from UI_DESC  
	m_halfspace(desc.halfspace), // Initialize m_halfspace from UI_DESC  
	m_sphere(desc.sphere), // Initialize m_sphere from UI_DESC  
	m_rc(desc.rc)
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

void UI::DrawDemoWindow()
{
	ImGui::ShowDemoWindow();
}

void UI::DrawWindow()
{
	
	static bool placeSphereMode = false;
	/*Vertex v1 = { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 1.0f) };
	Vertex v2 = { glm::vec3(0.0f, 0.4f, 0.0f), glm::vec3(0.0f, 1.0f, 1.0f) };
	Vertex v3 = { glm::vec3(0.4f, 0.4f, 0.0f), glm::vec3(0.0f, 1.0f, 1.0f) };
	Vertex v4 = { glm::vec3(0.4f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 1.0f) };
	std::vector<Vertex> vertices = { v1, v2, v3, v4 };*/
	

	ImGui::Begin("Plotting Options");
		if (ImGui::Button("halfspaces")) {
			gui_mode = 0;
		}
		ImGui::SameLine();
		if (ImGui::Button("points")) {
			gui_mode = 1;
		}
		ImGui::SameLine();
		if (ImGui::Button("spheres")) {
			gui_mode = 2;
		}
		
		std::vector<std::string> items = { currentOption,"Translate", "Scale","RotateX", "RotateY","RotateZ"};
		int selectedDropdownIndex = 0;
		
		const char* previewValue = items[selectedDropdownIndex].c_str(); // Get the string for the selected item
		if (ImGui::BeginCombo("Drag and Transform Options", previewValue))
		{
			for (int i = 0; i < items.size(); ++i)
			{
				const bool isSelected = (selectedDropdownIndex == i);
				if (ImGui::Selectable(items[i].c_str(), isSelected))
				{
					selectedDropdownIndex = i; // Update the selected item
					currentOption = items[i].c_str();
				}

				// Set the initial focus when opening the combo (optional)
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
				
			}
			ImGui::EndCombo();
		}

		switch (gui_mode) {
		case 0:
				
				ImGui::Text("Type in hyperplane coefficients:");
				ImGui::InputText("a1", data.input1, IM_ARRAYSIZE(data.input1));
				ImGui::InputText("a2", data.input2, IM_ARRAYSIZE(data.input2));
				ImGui::InputText("a3", data.input3, IM_ARRAYSIZE(data.input3));
				ImGui::ColorEdit3("Color", (float*)&clear_color);

				/*if (ImGui::Button("Color")) {
					std::cout << "color " << clear_color.w << " " << clear_color.x << " " << clear_color.z << " " << clear_color.z << std::endl;
				}*/
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
					SnapToPosition(previewPos);
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
				ImGui::SetNextItemWidth(75.0f);
				ImGui::InputInt("n", &n_input);
				ImGui::SameLine();
				if (ImGui::Button("generate n-sided polygon"))
				{
					float radius = 0.2f;
					float theta = 2 * M_PI/n_input;
					int num = 0;
					std::vector<Point> polyverts;
					for (int i = 0; i < n_input; ++i) {
						double currentAngle = i * theta;
						Point p;
						p.x = 0.0f + radius * std::cos(currentAngle);
						p.y = 0.0f + radius * std::sin(currentAngle);
						polyverts.push_back(p);
					}
					for (const auto& v : polyverts) {
						num++;
						m_points->addInstance(v.x, v.y, 0.0f, glm::vec3(clear_color.x, clear_color.y, clear_color.z));
					}
					std::cout << num << " points added." << std::endl;
					m_points->load();

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
						maxdepth = TC.max_depth;
						
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
				if (maxdepth != -1)
				{
					ImGui::Text("MaxDepth %d: ", maxdepth);
				}
				if (ImGui::Button("Symmetric Projection")) {
					std::vector<Vertex> selected_points;
					if (selection && !currentSelections[GT.POINTS].selectedIndices.empty()) {
						for (int idx : currentSelections[GT.POINTS].selectedIndices) {
							selected_points.push_back(m_points->points[idx]);
						}
					}
					else {
						selected_points = m_points->points;
					}
					std::vector<glm::vec2> screen_space_points;
					for (const auto& pt : selected_points) {
						screen_space_points.push_back(glm::vec2(pt.position.z,pt.position.y));
						
						// The z-value here is irrelevant for the 2D algorithm
						//screen_space_points.push_back(Vertex{ glm::vec3(screenPos.x, screenPos.y, 0.0f), pt.color });
					}
					ProjectionResult P = TV_Projection(screen_space_points);
					std::vector<Vertex> proj_verts;
					
					for (glm::vec2 p : P.adjusted_points)
					{
						m_points->addInstance(-1.8f, p.y, p.x, glm::vec3(0, 0.2f, 0.5f));
						
					}
					m_points->load();
					
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
						selection = false;
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
			switch(selectedDropdownIndex)
			{
			case 1: std::cout << " Translate selected." << std::endl;
				break;
			case 2:  std::cout << " Scale selected." << std::endl;
			}

		ImGui::End();
		
}

void UI::DrawInspectorWindow(int opt)
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
		if (select_mode) {
			ImGui::Text("select mode = true");
		}
		else {
			ImGui::Text("select mode = false");
		}
	
		
		//ImGui::Text("Selections: (%.1f , %.1f, %.1f)", mousePos.x, mousePos.y, glm::length(previewDepth));
		ImGui::End();
		data_pointer = ImGui::GetDrawData();
	}
	if (opt == 2)
	{
		//ImGui::SetNextWindowPos(ImVec2(100, 50), ImGuiCond_FirstUseEver);
		ImGui::Begin("Simulation Setup", nullptr, ImGuiWindowFlags_MenuBar);
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				// This is the menu item that will trigger the save dialog
				if (ImGui::MenuItem("Save Simulation Data (.csv)...")) {
					ImGui::OpenPopup("Save Data Popup");
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		
		
		
		Selection& sel = currentSelections[0];
		if (ImGui::Button("Select Normal Agents")) {
			ImGui::Text("Selected Instances: %d", sel.selectedIndices.size());
			if (sel.selectedIndices.size() > 0) {
				for (int idx : sel.selectedIndices) {
					normalAgents.push_back(idx);
					
				}
				
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Select Adversarial Agents")) {

			ImGui::Text("Selected Instances: %d", sel.selectedIndices.size());
			if (sel.selectedIndices.size() > 0) {
				for (int idx : sel.selectedIndices) {
					adversaries.push_back(idx);
				}
				
			}
		}
		for (int idx : normalAgents) {
			glm::vec3 worldPos = sel.m_geometryType->getInstanceWorldCoords(idx);
			
			ImGui::Text("Normal Agent %d: (%.2f, %.2f, %.2f)", idx, worldPos.x, worldPos.y, worldPos.z);
		}
		for (int idx : adversaries) {
			glm::vec3 worldPos = sel.m_geometryType->getInstanceWorldCoords(idx);

			ImGui::Text("Adversaries %d: (%.2f, %.2f, %.2f)", idx, worldPos.x, worldPos.y, worldPos.z);
		}
		if (ImGui::Button("Run Simulation")) {
		
			//m_rc = std::make_shared<ResilientConsensus>(normalAgents, adversaries, m_points);

			//m_rc->start_sim(100);
			sim_running = true;
		}
		ImGui::Checkbox("Set Capture Area", &set_capture_area_mode);

		if (select_mode || set_capture_area_mode) {
			drawSelectionBox();
		}
		// --- 2. Define the Popup Modal Window ---
		
		ImGui::End();
		data_pointer = ImGui::GetDrawData();
	}
}
//glm::vec3 UI::getPreviewPos()
//{
//	
//	ImVec2 mousePos = ImGui::GetMousePos();
//	
//	previewPos = (mousePos.x / cameraData.scr_width * 2.0f - 1.0f) * cameraData.Right
//		- (mousePos.y / cameraData.scr_height * 2.0f - 1.0f) * cameraData.Up;
//
//	previewPos += previewDepth;
//	previewPos = glm::vec(previewPos);
//	return previewPos;
//	
//}
void UI::DrawPopups()
{
	// Set the position for the popup just before we attempt to open it.
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	// Now, define the popup.
	if (ImGui::BeginPopupModal("Save Data Popup", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		// This will now print correctly.
		std::cout << "popup should appear" << std::endl;

		ImGui::Text("Enter the full path to save the simulation data:");
		ImGui::InputText("##savepath", save_path, IM_ARRAYSIZE(save_path));
		ImGui::Separator();

		if (ImGui::Button("Save", ImVec2(120, 0))) {
			if (m_rc && !m_rc->X_history.empty()) {
				saveSimulationData(save_path, m_rc->X_history);
			}
			else {
				std::cerr << "No simulation data available to save." << std::endl;
			}
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}



glm::vec3 UI::getPreviewPos()
{
	// 1. Get the current mouse position from ImGui.
	ImVec2 mousePos = ImGui::GetMousePos();

	// 2. Define a default depth. 0.5f places the point halfway 
	//    between the near and far clipping planes.
	float winZ = 0.5f;

	previewPos = UIscreenToWorld(
		glm::vec2(mousePos.x, mousePos.y),
		winZ,
		glm::mat4(1.0f),
		view,
		proj,
		scr_width,
		scr_height
	);
	previewPos += previewDepth;
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
	if (set_capture_area_mode) {
		if (ImGui::IsMouseClicked(0))
		{
			startPos = ImGui::GetMousePos();
		}
		if (ImGui::IsMouseDragging(0) && !ImGui::IsAnyItemHovered()) {
			
			dragging = true;
			currentPos = ImGui::GetMousePos();
			ImDrawList* draw_list = ImGui::GetForegroundDrawList();
			// Draw a red rectangle to indicate capture mode
			draw_list->AddRect(startPos, currentPos, IM_COL32(255, 0, 0, 255), 0.0f, 0, 2.0f);
		}
		
		if (ImGui::IsMouseReleased(0)&& dragging) {
			dragging = false;
			// Store the coordinates and set the flag
			capture_start_pos = startPos;
			capture_end_pos = ImGui::GetMousePos();
			capture_area_set = true;
			startPos = ImVec2(0, 0);
			endPos = ImVec2(0, 0);
			//set_capture_area_mode = false;
		}
	}
	else {
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
		if (!ImGui::IsMouseDragging(0) && !dragging && !selection)
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
	if (capture_area_set) {
		
		ImDrawList* draw_list = ImGui::GetForegroundDrawList();
		draw_list->AddRect(capture_start_pos, capture_end_pos, IM_COL32(255, 255, 0, 255), 0.0f, 0, 2.0f);
		if (ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered)
		{
			capture_area_set = false;
		}
	}
}
void UI::highlightHoverSelect(ImVec2 p0) {


	// Get the min/max corners of the selection box
	float minX = p0.x - 5.0, maxX = p0.x + 5.0;
	float minY = p0.y - 5.0, maxY = p0.y + 5.0;
	float r;
	glm::vec3 cursor_world_pos = getPreviewPos();
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
					std::cout << " point " << idx << " unhighlighted." << std::endl;
				}
				break;
			case 1: //Spheres

				glm::vec2 r_c = UIworldToScreen(worldPos + glm::vec3(0.0f, (float)m_sphere->sizes[idx].x, 0.0f), view, glm::mat4(1.0f), proj, scr_width, scr_height); // Assuming uniform size for spheres
				r = glm::distance(screenPos, r_c);


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
			case 2: //polygons

				
			
				
				if (m_polygon->HighlightPolygonIfHovered(idx, glm::vec3(p0.x,p0.y,0.0f), view, proj, scr_width, scr_height))
				{
					
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
					for (int s : sel.selectedIndices) {
						m_polygon->selected_indices[s] = true;
					}
					
				}
				else if (sel2.selectedIndices.size() > 0 && ImGui::IsMouseClicked(0))
				{	
					for (int s : sel2.selectedIndices) {
						m_polygon->selected_indices[s] = false;
					}
					
					// If the object is hovered but was not previously selected, add it to selection
					sel.selectedIndices.clear();
					sel2.selectedIndices.clear();
					std::cout << "selections cleared" << std::endl;

				}
				else if ((std::find(sel.selectedIndices.begin(), sel.selectedIndices.end(), idx) != sel.selectedIndices.end())
					&& !(std::find(sel2.selectedIndices.begin(), sel2.selectedIndices.end(), idx) != sel2.selectedIndices.end()))
				{
					for (int s : sel.selectedIndices) {
						m_polygon->selected_indices[s] = false;
					}
					// If the object is not hovered but was previously selected, remove it from selection
					auto it = std::remove(sel.selectedIndices.begin(), sel.selectedIndices.end(), idx);
					sel.selectedIndices.erase(it, sel.selectedIndices.end());
					
					//sel.m_geometryType->unhighlight_selected({ idx });
				}

				else {
					sel.selectedIndices.clear();
				}
				break;
			}
			

			sel.m_geometryType->highlight_selected(sel.selectedIndices);

		}
	}
}

void UI::SnapToPosition(glm::vec3& previewPos)
{
	ImDrawList* draw_list = ImGui::GetForegroundDrawList();
	const float snap_threshold_xy = 15.0f; // Snap sensitivity for X and Y in screen pixels
	const float snap_threshold_z = 0.01f;  // Snap sensitivity for Z in normalized device coordinates
	bool snapped_x = false;
	bool snapped_y = false;
	bool snapped_z = false;

	// Project the current preview position to screen space to get its original screen coords and depth
	glm::mat4 mvp = proj * view * glm::mat4(1.0f);
	glm::vec4 clipPos = mvp * glm::vec4(previewPos, 1.0f);
	float original_winZ = ((clipPos.z / clipPos.w) + 1.0f) * 0.5f; // Original window depth for unprojection

	glm::vec2 previewScreenPos = UIworldToScreen(previewPos, glm::mat4(1.0f), view, proj, scr_width, scr_height);

	// These will hold the final snapped coordinates
	glm::vec2 snappedScreenPos = previewScreenPos;
	float snapped_winZ = original_winZ;

	// Iterate through all selectable object types
	for (const auto& sel : currentSelections) {
		int instance_count = sel.m_geometryType->getInstanceCount();
		for (int i = 0; i < instance_count; ++i) {
			// Don't snap to the preview object itself
			if (previewInstance && i == preview_idx && sel.m_geometryType == m_points) continue;
			if (previewInstance && i == preview_idx && sel.m_geometryType == m_sphere) continue;

			glm::vec3 worldPos = sel.m_geometryType->getInstanceWorldCoords(i);
			glm::vec2 objectScreenPos = UIworldToScreen(worldPos, glm::mat4(1.0f), view, proj, scr_width, scr_height);

			// Calculate the object's window depth
			glm::vec4 objectClipPos = mvp * glm::vec4(worldPos, 1.0f);
			float object_winZ = ((objectClipPos.z / objectClipPos.w) + 1.0f) * 0.5f;

			float dx = std::abs(previewScreenPos.x - objectScreenPos.x);
			float dy = std::abs(previewScreenPos.y - objectScreenPos.y);
			float dz = std::abs(original_winZ - object_winZ);

			// Check for horizontal (Y-axis) snap
			if (dy < snap_threshold_xy) {
				snappedScreenPos.y = objectScreenPos.y;
				draw_list->AddLine(
					ImVec2(objectScreenPos.x, objectScreenPos.y),
					ImVec2(snappedScreenPos.x, snappedScreenPos.y),
					IM_COL32(0, 255, 0, 255), 1.5f);
				snapped_y = true;
			}

			// Check for vertical (X-axis) snap
			if (dx < snap_threshold_xy) {
				snappedScreenPos.x = objectScreenPos.x;
				draw_list->AddLine(
					ImVec2(objectScreenPos.x, objectScreenPos.y),
					ImVec2(snappedScreenPos.x, snappedScreenPos.y),
					IM_COL32(0, 255, 0, 255), 1.5f);
				snapped_x = true;
			}

			// ? Check for depth (Z-axis) snap
			if (dz < snap_threshold_z) {
				snapped_winZ = object_winZ; // Snap the depth value
				// Draw a circle on the aligned object to indicate a Z-snap
				draw_list->AddCircle(ImVec2(objectScreenPos.x, objectScreenPos.y), 12.0f, IM_COL32(0, 255, 0, 255), 12, 2.0f);
				snapped_z = true;
			}
		}
	}

	// If any snap occurred, unproject the new screen position back to world space
	if (snapped_x || snapped_y || snapped_z) {
		previewPos = UIscreenToWorld(snappedScreenPos, snapped_winZ, glm::mat4(1.0f), view, proj, scr_width, scr_height);
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
				selection = true;
			}
		}
		
		//m_sphere->highlight_selected(std::vector<int>(selectedSpheres.begin(), selectedSpheres.end()));
		sel.m_geometryType->highlight_selected(sel.selectedIndices);
		if (i == 0) {
			for (int j : sel.selectedIndices) {
				std::cout << j << " selected and highlighted." << std::endl;
			}
		}
		
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
void UI::saveSimulationData(const std::string& filepath, const std::vector<std::vector<glm::vec2>>& history) {
	if (history.empty()) {
		std::cerr << "Simulation history is empty. Nothing to save." << std::endl;
		return;
	}

	// Open the file for writing
	std::ofstream csv_file(filepath);
	if (!csv_file.is_open()) {
		std::cerr << "Failed to open file for writing: " << filepath << std::endl;
		return;
	}

	// --- Write the CSV Header ---
	csv_file << "step";
	int num_agents = history[0].size();
	for (int i = 0; i < num_agents; ++i) {
		csv_file << ",agent_" << i << "_x,agent_" << i << "_y";
	}
	csv_file << "\n";

	for (size_t step = 0; step < history.size(); ++step) {
		csv_file << step; // First column is the step number
		for (int agent_idx = 0; agent_idx < num_agents; ++agent_idx) {
			// Add the x and y coordinates for each agent
			csv_file << "," << history[step][agent_idx].x << "," << history[step][agent_idx].y;
		}
		csv_file << "\n";
	}

	csv_file.close();
	std::cout << "Successfully saved simulation data to " << filepath << std::endl;
}


bool UI::sort_descend(int a, int b) {
	return a > b;
}
bool UI::sort_ascend(int a, int b) {
	return a < b;
}