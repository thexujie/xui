#include "stdafx.h"
#include "x/platform/win32/win32.h"

using core::operator<<;

int main()
{
#if defined _DEBUG
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
    core::App app;

	drawing::path_source path_source;
	path_source.move({ 0.0f, 0.0f });
	path_source.line({ 0.0f, 0.0f });
	path_source.line({ 0.0f, 0.0f });
	path_source.line({ 0.0f, 0.0f });
	path_source.line({ 0.0f, 0.0f });
	path_source.arc({ 0.0f, 0.0f }, { 0.0f, 0.0f }, 12);
	
	core::xml xml;
	xml.load(u8"../data/layout.xml");
	auto root = xml.root();
	auto nodes = xml.root().children();
	auto root_name = root.name();

	auto node = root.find(u8"stack/div/row/te1xt")[u8"text"].value();
	
	auto titlebar = root.child(u8"titlebar");
	for (auto & node : titlebar.children(u8"button"))
	{
		std::cout << node.name() << std::endl;
		for (auto & attr : node.attributes_generator())
			std::cout << attr.name() << std::endl;
	}
	
	for (auto & node : root.children_generator())
	{
		std::cout << node.name() << std::endl;
		for (auto & attr : node.attributes_generator())
			std::cout << attr.name() << std::endl;
	}
	
	return 0;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    //https://docs.microsoft.com/en-us/windows/desktop/api/shellscalingapi/nf-shellscalingapi-setprocessdpiawareness
    // 使用 manifest 而不是直接设置
    //HMODULE Shcore = GetModuleHandleW(L"Shcore.dll");
    //auto pfn_SetProcessDpiAwareness = reinterpret_cast<decltype(SetProcessDpiAwareness) *>(GetProcAddress(Shcore, "SetProcessDpiAwareness"));
    //if(pfn_SetProcessDpiAwareness)
    //	pfn_SetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE);

    main();
    return 0;
}
