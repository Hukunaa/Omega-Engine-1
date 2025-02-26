﻿/*
MIT License

Copyright (c) 2019-2020 Zhuang Guan

https://github.com/AirGuanZ

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include <array>
#include <cstring>
#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#ifndef IMGUI_VERSION
#   error "include imgui.h before this header"
#endif

using ImGuiFileBrowserFlags = int;

enum ImGuiFileBrowserFlags_
{
	ImGuiFileBrowserFlags_SelectDirectory = 1 << 0, // select directory instead of regular file
	ImGuiFileBrowserFlags_EnterNewFilename = 1 << 1, // allow user to enter new filename when selecting regular file
	ImGuiFileBrowserFlags_NoModal = 1 << 2, // file browsing window is modal by default. specify this to use a popup window
	ImGuiFileBrowserFlags_NoTitleBar = 1 << 3, // hide window title bar
	ImGuiFileBrowserFlags_NoStatusBar = 1 << 4, // hide status bar at the bottom of browsing window
	ImGuiFileBrowserFlags_CloseOnEsc = 1 << 5, // close file browser when pressing 'ESC'
	ImGuiFileBrowserFlags_CreateNewDir = 1 << 6, // allow user to create new directory
};

namespace ImGui
{
	class FileBrowser
	{
	public:

		// pwd is set to current working directory by default
		explicit FileBrowser(ImGuiFileBrowserFlags flags = 0);

		FileBrowser(const FileBrowser& copyFrom);

		FileBrowser& operator=(const FileBrowser& copyFrom);

		// set the window size (in pixels)
		// default is (700, 450)
		void SetWindowSize(int width, int height) noexcept;

		// set the window title text
		void SetTitle(std::string title);

		// set the opening mode
		void EnableSaveMode(const bool enabledMode);

		// open the browsing window
		void Open();

		// close the browsing window
		void Close();

		// the browsing window is opened or not
		bool IsOpened() const noexcept;

		// the browsing window is working as a save window or a load window
		bool SavingMode() const noexcept;

		// display the browsing window if opened
		void Display();

		// returns true when there is a selected filename and the "ok" button was clicked
		bool HasSelected() const noexcept;

		// set current browsing directory
		bool SetPwd(const std::filesystem::path& pwd =
			std::filesystem::current_path());

		// returns selected filename. make sense only when HasSelected returns true
		std::filesystem::path GetSelected() const;

		// set selected filename to empty
		void ClearSelected();

		// set file type filters. eg. { ".h", ".cpp", ".hpp", ".cc", ".inl" }
		void SetTypeFilters(const std::vector<const char*>& typeFilters);

	private:

		class ScopeGuard
		{
			std::function<void()> func_;

		public:

			template<typename T>
			explicit ScopeGuard(T func) : func_(std::move(func)) { }
			~ScopeGuard() { func_(); }
		};

		void SetPwdUncatched(const std::filesystem::path& pwd);
		bool HasSpecialChar(const std::string& p_stringToCheck) const;

#ifdef _WIN32
		static std::uint32_t GetDrivesBitMask();
#endif

		int width_;
		int height_;
		ImGuiFileBrowserFlags flags_;

		std::string title_;
		std::string openLabel_;

		bool openFlag_;
		bool closeFlag_;
		bool isOpened_;
		bool ok_;
		bool saveMode_;

		std::string statusStr_;

		char sceneNameInput_[32];
		std::string sceneName_;

		std::vector<const char*> typeFilters_;
		int typeFilterIndex_;

		std::filesystem::path pwd_;
		std::filesystem::path selectedFilename_;

		struct FileRecord
		{
			bool isDir = false;
			std::filesystem::path name;
			std::string showName;
			std::filesystem::path extension;
		};
		std::vector<FileRecord> fileRecords_;

		// IMPROVE: truncate when selectedFilename_.length() > inputNameBuf_.size() - 1
		static constexpr size_t INPUT_NAME_BUF_SIZE = 512;
		std::unique_ptr<std::array<char, INPUT_NAME_BUF_SIZE>> inputNameBuf_;

		std::string openNewDirLabel_;
		std::unique_ptr<std::array<char, INPUT_NAME_BUF_SIZE>> newDirNameBuf_;

#ifdef _WIN32
		uint32_t drives_;
#endif
	};
} // namespace ImGui

inline ImGui::FileBrowser::FileBrowser(ImGuiFileBrowserFlags flags)
	: width_(700), height_(450), flags_(flags),
	openFlag_(false), closeFlag_(false), isOpened_(false), ok_(false), saveMode_(true),
	inputNameBuf_(std::make_unique<std::array<char, INPUT_NAME_BUF_SIZE>>())
{
	if (flags_ & ImGuiFileBrowserFlags_CreateNewDir)
		newDirNameBuf_ = std::make_unique<
		std::array<char, INPUT_NAME_BUF_SIZE>>();

	inputNameBuf_->at(0) = '\0';
	SetTitle("file browser");
	SetPwd(std::filesystem::current_path());

	typeFilters_.clear();
	typeFilterIndex_ = 0;

	strcpy_s(sceneNameInput_, "SampleScene.omega");
	sceneName_ = sceneNameInput_;

#ifdef _WIN32
	drives_ = GetDrivesBitMask();
#endif
}

inline ImGui::FileBrowser::FileBrowser(const FileBrowser& copyFrom)
	: FileBrowser()
{
	*this = copyFrom;
}

inline ImGui::FileBrowser& ImGui::FileBrowser::operator=(
	const FileBrowser& copyFrom)
{
	if (this == &copyFrom)
		return *this;

	flags_ = copyFrom.flags_;
	SetTitle(copyFrom.title_);

	openFlag_ = copyFrom.openFlag_;
	closeFlag_ = copyFrom.closeFlag_;
	isOpened_ = copyFrom.isOpened_;
	ok_ = copyFrom.ok_;
	saveMode_ = copyFrom.saveMode_;

	statusStr_ = "";
	pwd_ = copyFrom.pwd_;
	selectedFilename_ = copyFrom.selectedFilename_;
	strcpy_s(sceneNameInput_, copyFrom.sceneNameInput_);
	sceneName_ = copyFrom.sceneName_;

	fileRecords_ = copyFrom.fileRecords_;

	*inputNameBuf_ = *copyFrom.inputNameBuf_;

	if (flags_ & ImGuiFileBrowserFlags_CreateNewDir)
	{
		newDirNameBuf_ = std::make_unique<
			std::array<char, INPUT_NAME_BUF_SIZE>>();
		*newDirNameBuf_ = *copyFrom.newDirNameBuf_;
	}

	return *this;
}

inline void ImGui::FileBrowser::SetWindowSize(int width, int height) noexcept
{
	assert(width > 0 && height > 0);
	width_ = width;
	height_ = height;
}

inline void ImGui::FileBrowser::SetTitle(std::string title)
{
	title_ = std::move(title);
	openLabel_ = title_ + "##filebrowser_" +
		std::to_string(reinterpret_cast<size_t>(this));
	openNewDirLabel_ = "new dir##new_dir_" +
		std::to_string(reinterpret_cast<size_t>(this));
}

inline void ImGui::FileBrowser::EnableSaveMode(const bool enabledMode)
{
	saveMode_ = enabledMode;
}

inline void ImGui::FileBrowser::Open()
{
	ClearSelected();
	statusStr_ = std::string();
	openFlag_ = true;
	closeFlag_ = false;
}

inline void ImGui::FileBrowser::Close()
{
	ClearSelected();
	statusStr_ = std::string();
	closeFlag_ = true;
	openFlag_ = false;
}

inline bool ImGui::FileBrowser::IsOpened() const noexcept
{
	return isOpened_;
}

inline bool ImGui::FileBrowser::SavingMode() const noexcept
{
	return saveMode_;
}

inline void ImGui::FileBrowser::Display()
{
	PushID(this);
	ScopeGuard exitThis([this]
		{
			openFlag_ = false;
			closeFlag_ = false;
			PopID();
		});

	if (openFlag_)
		OpenPopup(openLabel_.c_str());
	isOpened_ = false;

	// open the popup window

	if (openFlag_ && (flags_ & ImGuiFileBrowserFlags_NoModal))
	{
		SetNextWindowSize(
			ImVec2(static_cast<float>(width_), static_cast<float>(height_)));
	}
	else
	{
		SetNextWindowSize(
			ImVec2(static_cast<float>(width_), static_cast<float>(height_)),
			ImGuiCond_FirstUseEver);
	}
	if (flags_ & ImGuiFileBrowserFlags_NoModal)
	{
		if (!BeginPopup(openLabel_.c_str()))
			return;
	}
	else if (!BeginPopupModal(openLabel_.c_str(), nullptr,
		flags_ & ImGuiFileBrowserFlags_NoTitleBar ?
		ImGuiWindowFlags_NoTitleBar : 0))
	{
		return;
	}
	isOpened_ = true;
	ScopeGuard endPopup([] { EndPopup(); });

	// display elements in pwd

#ifdef _WIN32
	char currentDrive = static_cast<char>(pwd_.c_str()[0]);
	char driveStr[] = { currentDrive, ':', '\0' };

	PushItemWidth(4 * GetFontSize());
	if (BeginCombo("##select_drive", driveStr))
	{
		ScopeGuard guard([&] { ImGui::EndCombo(); });
		for (int i = 0; i < 26; ++i)
		{
			if (!(drives_ & (1 << i)))
				continue;
			char driveCh = static_cast<char>('A' + i);
			char selectableStr[] = { driveCh, ':', '\0' };
			bool selected = currentDrive == driveCh;
			if (Selectable(selectableStr, selected) && !selected)
			{
				char newPwd[] = { driveCh, ':', '\\', '\0' };
				SetPwd(newPwd);
			}
		}
	}
	PopItemWidth();

	SameLine();
#endif

	int secIdx = 0, newPwdLastSecIdx = -1;
	for (auto& sec : pwd_)
	{
#ifdef _WIN32
		if (secIdx == 1)
		{
			++secIdx;
			continue;
		}
#endif
		PushID(secIdx);
		if (secIdx > 0)
			SameLine();
		if (SmallButton(sec.u8string().c_str()))
			newPwdLastSecIdx = secIdx;
		PopID();
		++secIdx;
	}

	if (newPwdLastSecIdx >= 0)
	{
		int i = 0;
		std::filesystem::path newPwd;
		for (auto& sec : pwd_)
		{
			if (i++ > newPwdLastSecIdx)
				break;
			newPwd /= sec;
		}
#ifdef _WIN32
		if (newPwdLastSecIdx == 0)
			newPwd /= "\\";
#endif
		SetPwd(newPwd);
	}

	SameLine();

	if (SmallButton("*"))
		SetPwd(pwd_);

	if (newDirNameBuf_)
	{
		SameLine();
		if (SmallButton("+"))
		{
			OpenPopup(openNewDirLabel_.c_str());
			(*newDirNameBuf_)[0] = '\0';
		}

		if (BeginPopup(openNewDirLabel_.c_str()))
		{
			ScopeGuard endNewDirPopup([] { EndPopup(); });

			InputText("name", newDirNameBuf_->data(), newDirNameBuf_->size());
			SameLine();

			if (Button("Ok") && (*newDirNameBuf_)[0] != '\0')
			{
				ScopeGuard closeNewDirPopup([] { CloseCurrentPopup(); });
				if (create_directory(pwd_ / newDirNameBuf_->data()))
					SetPwd(pwd_);
				else
				{
					statusStr_ = "failed to create " +
						std::string(newDirNameBuf_->data());
				}
			}
		}
	}

	// browse files in a child window

	float reserveHeight = GetFrameHeightWithSpacing();
	std::filesystem::path newPwd; bool setNewPwd = false;
	if (!(flags_ & ImGuiFileBrowserFlags_SelectDirectory) &&
		(flags_ & ImGuiFileBrowserFlags_EnterNewFilename))
		reserveHeight += GetFrameHeightWithSpacing();
	{
		BeginChild("ch", ImVec2(0, -reserveHeight), true,
			(flags_ & ImGuiFileBrowserFlags_NoModal) ?
			ImGuiWindowFlags_AlwaysHorizontalScrollbar : 0);
		ScopeGuard endChild([] { EndChild(); });

		for (auto& rsc : fileRecords_)
		{
			if (!rsc.isDir && typeFilters_.size() > 0 &&
				static_cast<size_t>(typeFilterIndex_) < typeFilters_.size() &&
				!(rsc.extension == typeFilters_[typeFilterIndex_]))
				continue;

			if (!rsc.name.empty() && rsc.name.c_str()[0] == '$')
				continue;

			const bool selected = selectedFilename_ == rsc.name;
			if (Selectable(rsc.showName.c_str(), selected,
				ImGuiSelectableFlags_DontClosePopups))
			{
				if (selected)
				{
					selectedFilename_ = std::filesystem::path();
					(*inputNameBuf_)[0] = '\0';
				}
				else if (rsc.name != "..")
				{
					if ((rsc.isDir && (flags_ & ImGuiFileBrowserFlags_SelectDirectory)) ||
						(!rsc.isDir && !(flags_ & ImGuiFileBrowserFlags_SelectDirectory)))
					{
						selectedFilename_ = rsc.name;
						if (!(flags_ & ImGuiFileBrowserFlags_SelectDirectory))
						{
#ifdef _MSC_VER
							strcpy_s(inputNameBuf_->data(), inputNameBuf_->size(),
								selectedFilename_.u8string().c_str());
#else
							std::strncpy(inputNameBuf_->data(), selectedFilename_.u8string().c_str(),
								inputNameBuf_->size());
#endif
						}
					}
				}
			}

			if (IsItemClicked(0) && IsMouseDoubleClicked(0) && rsc.isDir)
			{
				setNewPwd = true;
				newPwd = (rsc.name != "..") ? (pwd_ / rsc.name) :
					pwd_.parent_path();
			}
		}
	}

	if (setNewPwd)
		SetPwd(newPwd);

	if (!(flags_ & ImGuiFileBrowserFlags_SelectDirectory) &&
		(flags_ & ImGuiFileBrowserFlags_EnterNewFilename))
	{
		PushID(this);
		ScopeGuard popTextID([] { PopID(); });

		PushItemWidth(-1);
		if (InputText("", inputNameBuf_->data(), inputNameBuf_->size()))
			selectedFilename_ = inputNameBuf_->data();
		PopItemWidth();
	}

	if (SavingMode())
	{
		if (!selectedFilename_.empty())
		{
			sceneName_ = selectedFilename_.filename().string();
			strcpy_s(sceneNameInput_, sceneName_.data());
		}

		if (ImGui::InputText("##saveName", sceneNameInput_, sizeof(sceneNameInput_), ImGuiInputTextFlags_AutoSelectAll))
		{
			sceneName_ = std::string(sceneNameInput_);
			if (HasSpecialChar(sceneName_))
			{
				// filename is not syntaxically correct
				sceneName_ = "";
				sceneNameInput_[0] = '\0';
			}
			if (!sceneName_.empty())
			{
				const auto pointFound = sceneName_.find_first_of('.');
				if (pointFound != std::string::npos && pointFound < 32 - 7)
				{
					const auto omega = sceneName_.find(".omega");
					if (omega == std::string::npos)
					{
						// We find a point, which indicate the begining of extension, we overwrite there with .omega
						sceneName_ = sceneName_.substr(0, pointFound);
						sceneName_ += ".omega";
						strcpy_s(sceneNameInput_, sceneName_.data());
					}
					else
					{
						sceneName_ = sceneName_.substr(0, omega + 6);
						strcpy_s(sceneNameInput_, sceneName_.data());
					}
				}
				else
				{
					if (sceneName_.size() < 32 - 7)
					{
						sceneName_ = sceneName_ + ".omega";
						strcpy_s(sceneNameInput_, sceneName_.data());
					}
					else
					{
						sceneName_ = sceneName_.substr(0, 32 - 7);
						sceneName_ = sceneName_ + ".omega";
						strcpy_s(sceneNameInput_, sceneName_.data());
					}
				}
			}
			selectedFilename_ = std::string();
		}
		SameLine();
	}

	if (!(flags_ & ImGuiFileBrowserFlags_SelectDirectory))
	{
		if (Button(" Ok "))
		{
			if (!selectedFilename_.empty())
			{
				ok_ = true;
				CloseCurrentPopup();
			}
			else if (!sceneName_.empty())
			{
				std::ofstream createFile;
				createFile.open(sceneName_, std::ios::out | std::ios::trunc);
				if (createFile.is_open())
				{
					std::filesystem::path pathOF(sceneName_);
					selectedFilename_ = pathOF;
				}
				createFile.close();

				ok_ = true;
				CloseCurrentPopup();
			}
		}
	}
	else
	{
		if (selectedFilename_.empty())
		{
			if (Button(" Ok "))
			{
				ok_ = true;
				CloseCurrentPopup();
			}
		}
		else if (Button("Open"))
			SetPwd(pwd_ / selectedFilename_);
	}

	SameLine();

	int escIdx = GetIO().KeyMap[ImGuiKey_Escape];
	if (Button("Cancel") || closeFlag_ ||
		((flags_ & ImGuiFileBrowserFlags_CloseOnEsc) &&
			IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
			escIdx >= 0 && IsKeyPressed(escIdx)))
		CloseCurrentPopup();

	if (!statusStr_.empty() && !(flags_ & ImGuiFileBrowserFlags_NoStatusBar))
	{
		SameLine();
		Text("%s", statusStr_.c_str());
	}

	if (!typeFilters_.empty())
	{
		SameLine();
		PushItemWidth(8 * GetFontSize());
		Combo("##type_filters", &typeFilterIndex_,
			typeFilters_.data(), int(typeFilters_.size()));
		PopItemWidth();
	}
}

inline bool ImGui::FileBrowser::HasSelected() const noexcept
{
	return ok_;
}

inline bool ImGui::FileBrowser::SetPwd(const std::filesystem::path& pwd)
{
	try
	{
		SetPwdUncatched(pwd);
		return true;
	}
	catch (const std::exception& err)
	{
		statusStr_ = std::string("last error: ") + err.what();
	}
	catch (...)
	{
		statusStr_ = "last error: unknown";
	}

	SetPwdUncatched(std::filesystem::current_path());
	return false;
}

inline std::filesystem::path ImGui::FileBrowser::GetSelected() const
{
	return pwd_ / selectedFilename_;
}

inline void ImGui::FileBrowser::ClearSelected()
{
	selectedFilename_ = std::string();
	sceneName_ = std::string();
	sceneNameInput_[0] = '\0';
	(*inputNameBuf_)[0] = '\0';
	ok_ = false;
}

inline void ImGui::FileBrowser::SetTypeFilters(
	const std::vector<const char*>& typeFilters)
{
	typeFilters_ = typeFilters;
	typeFilterIndex_ = 0;
}

inline void ImGui::FileBrowser::SetPwdUncatched(const std::filesystem::path& pwd)
{
	fileRecords_ = { FileRecord{ true, "..", "[D] ..", "" } };

	for (auto& p : std::filesystem::directory_iterator(pwd))
	{
		FileRecord rcd;

		if (p.is_regular_file())
			rcd.isDir = false;
		else if (p.is_directory())
			rcd.isDir = true;
		else
			continue;

		rcd.name = p.path().filename();
		if (rcd.name.empty())
			continue;

		rcd.extension = p.path().filename().extension();

		rcd.showName = (rcd.isDir ? "[D] " : "[F] ") +
			p.path().filename().u8string();
		fileRecords_.push_back(rcd);
	}

	std::sort(fileRecords_.begin(), fileRecords_.end(),
		[](const FileRecord& L, const FileRecord& R)
		{
			return (L.isDir ^ R.isDir) ? L.isDir : (L.name < R.name);
		});

	pwd_ = absolute(pwd);
	selectedFilename_ = std::string();
	(*inputNameBuf_)[0] = '\0';
}

inline bool ImGui::FileBrowser::HasSpecialChar(const std::string& p_stringToCheck) const
{
	return std::find_if(p_stringToCheck.begin(), p_stringToCheck.end(),
		[](const char p_character)
		{
			return !(isalnum(p_character) || p_character == '_' || p_character == '.' || p_character == '-' || p_character == ' ' || p_character == '(' || p_character == ')');
		}) != p_stringToCheck.end();
}

#ifdef _WIN32

#ifndef _INC_WINDOWS

#ifndef WIN32_LEAN_AND_MEAN

#define IMGUI_FILEBROWSER_UNDEF_WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN

#endif // #ifndef WIN32_LEAN_AND_MEAN

#include <Windows.h>

#ifdef IMGUI_FILEBROWSER_UNDEF_WIN32_LEAN_AND_MEAN
#undef IMGUI_FILEBROWSER_UNDEF_WIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#endif // #ifdef IMGUI_FILEBROWSER_UNDEF_WIN32_LEAN_AND_MEAN

#endif // #ifdef _INC_WINDOWS

inline std::uint32_t ImGui::FileBrowser::GetDrivesBitMask()
{
	DWORD mask = GetLogicalDrives();
	uint32_t ret = 0;
	for (int i = 0; i < 26; ++i)
	{
		if (!(mask & (1 << i)))
			continue;
		char rootName[4] = { static_cast<char>('A' + i), ':', '\\', '\0' };
		UINT type = GetDriveTypeA(rootName);
		if (type == DRIVE_REMOVABLE || type == DRIVE_FIXED)
			ret |= (1 << i);
	}
	return ret;
}

#endif
