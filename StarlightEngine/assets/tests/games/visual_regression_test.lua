-- visual_regression_test.lua
-- Teste de Regressão Visual Integrado em Lua para o Fusion ENGINE

test.describe("Regressão Visual - Integridade de Renderização", function()
    -- Ignorar testes físicos se estiver rodando na suíte headless de testes automatizados do GTest
    if test.is_headless_test then
        Engine.log_info("AutomationSystem: Headless GTest environment detected. Bypassing physical visual regression test.")
        test.assert(true)
        return
    end

    -- 1. Verificar se conseguimos salvar um screenshot da tela atual
    -- (Em ambiente headless do GTest isso faz bypass e retorna true silenciosamente)
    local saveSuccess = test.save_screenshot("build/test_failures/test_render_sanity.tga")
    test.assert(saveSuccess == true, "Deve salvar o screenshot do frame atual")

    -- 2. Comparar o frame atual com a baseline visual unificada da engine
    -- O limite aceitável de divergência de pixels é de 1.5%
    -- Se a janela estiver ativa e os pixels divergirem do esperado, lança um erro estrito (Hard Failure)
    local compareSuccess = test.compare_screenshot(
        "assets/tests/baselines/engine_showcase_baseline.tga",
        "build/test_failures/engine_showcase_failure.tga",
        1.5
    )
    test.assert(compareSuccess == true, "A renderização gráfica do viewport deve bater com a baseline")
end)
