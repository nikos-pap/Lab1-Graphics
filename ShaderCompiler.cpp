#include "ShaderCompiler.h"
#include <slang-com-ptr.h> 

SlangCompiler::SlangCompiler()
{
	// Create a global session that can be reused.
	slang::createGlobalSession(&m_globalSession);
	if (!m_globalSession)
		throw std::runtime_error("Failed to create Slang global session.");
}

SlangCompiler::~SlangCompiler()
{
	if (m_globalSession)
		m_globalSession->release();
}

// Compile to GLSL text
std::string SlangCompiler::compileToGLSL(const std::string& source,
	const std::string& entryPoint,
	const std::string& stage)
{
	return compile(source, entryPoint, stage, SLANG_GLSL).asText();
}

// Compile to HLSL text
std::string SlangCompiler::compileToHLSL(const std::string& source,
	const std::string& entryPoint,
	const std::string& stage)
{
	return compile(source, entryPoint, stage, SLANG_HLSL).asText();
}

// Compile to SPIR-V binary
std::vector<uint8_t> SlangCompiler::compileToSPIRV(const std::string& source,
	const std::string& entryPoint,
	const std::string& stage)
{
	std::vector<uint8_t> result = compile(source, entryPoint, stage, SLANG_SPIRV).binaryData;
	return result;
}

ShaderOutput SlangCompiler::compile(const std::string& source,
	const std::string& entryPoint,
	const std::string& stage,
	SlangCompileTarget target)
{
	slang::SessionDesc sessionDesc = {};
	slang::TargetDesc targetDesc = {};
	targetDesc.format = target;
	targetDesc.profile = m_globalSession->findProfile("sm_6_0"); // default profile

	sessionDesc.targets = &targetDesc;
	sessionDesc.targetCount = 1;

	slang::ISession* session = nullptr;
	if (SLANG_FAILED(m_globalSession->createSession(sessionDesc, &session)))
		throw std::runtime_error("Failed to create Slang session.");

	slang::ICompileRequest* request = nullptr;
	m_globalSession->createCompileRequest(&request);
	if (!request)
		throw std::runtime_error("Failed to create Slang compile request.");

	// Add source as a translation unit
	int translationUnit = request->addTranslationUnit(SLANG_SOURCE_LANGUAGE_SLANG, "module");
	request->addTranslationUnitSourceString(translationUnit, "shader.slang", source.c_str());

	// Add entry point
	SlangStage slangStage = getSlangStage(stage);
	int entryPointIndex = request->addEntryPoint(translationUnit, entryPoint.c_str(), slangStage);

	// Set target
	int targetIndex = request->addCodeGenTarget(target);
	request->setTargetProfile(targetIndex, m_globalSession->findProfile("sm_6_0"));

	if (SLANG_FAILED(request->compile()))
	{
		const char* diag = request->getDiagnosticOutput();
		std::string err = diag ? diag : "Unknown Slang compilation error.";
		request->release();
		session->release();
		throw std::runtime_error(err);
	}

	// Get compiled result
	const void *codePtr = nullptr;
	uint64_t codeSize = 0;
	codePtr = request->getEntryPointCode(entryPointIndex, &codeSize);

	if (!codePtr) {
		request->release();
		session->release();
		throw std::runtime_error("Failed to get compiled code.");
	}

	ShaderOutput output;
	output.target = target;
	output.binaryData.assign(reinterpret_cast<const uint8_t*>(codePtr),
		reinterpret_cast<const uint8_t*>(codePtr) + codeSize);

	request->release();
	session->release();
	return output;
}

SlangStage SlangCompiler::getSlangStage(const std::string& stage)
{
	if (stage == "vertex") return SLANG_STAGE_VERTEX;
	if (stage == "fragment") return SLANG_STAGE_FRAGMENT;
	if (stage == "compute") return SLANG_STAGE_COMPUTE;
	throw std::runtime_error("Unsupported shader stage: " + stage);
}



// Example usage
#ifdef SLANG_COMPILER_EXAMPLE
int main()
{
    try
    {
        SlangCompiler compiler;
        std::string source = R"(
            [shader("vertex")]
            void main() { }
        )";

        std::string glsl = compiler.compileToGLSL(source, "main", "vertex");
        std::cout << "GLSL Output:\n" << glsl << "\n";

        std::vector<uint8_t> spirv = compiler.compileToSPIRV(source, "main", "vertex");
        std::cout << "SPIR-V size: " << spirv.size() << " bytes\n";

        std::string hlsl = compiler.compileToHLSL(source, "main", "vertex");
        std::cout << "HLSL Output:\n" << hlsl << "\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
#endif
