#pragma once
#ifndef AnsysLicenseChecker_H
#define AnsysLicenseChecker_H

#include <Windows.h>
#include <string>
#include <vector>

// Control IDs
constexpr int ID_LISTBOX = 101;
constexpr int ID_CHECK_BUTTON = 102;

// UI helpers
void PopulateLicenseList(HWND hListBox);

// License query stub (replace with your real implementation)
std::vector<std::wstring> GetAvailableLicenses();

#endif // AnsysLicenseChecker_H


