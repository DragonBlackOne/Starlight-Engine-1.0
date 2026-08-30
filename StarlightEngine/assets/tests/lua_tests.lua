test.describe("Matematica Basica em Lua", function()
    test.assert(2 + 2 == 4, "Soma simples deve ser verdadeira")
    test.assertEqual("Starlight", "Starlight", "Strings iguais devem passar no assertEqual")
end)

test.describe("Estruturas Globais da Engine", function()
    test.assert(Engine ~= nil, "A tabela global 'Engine' deve estar presente no estado Lua")
end)
