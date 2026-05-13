-- ==========================================
-- Vamos Engine - Script Lua Teste
-- Módulo testando a nova Lua API Bridge (AAA)
-- ==========================================

-- Isso gira o objeto vinculado
transform.rotation.z = transform.rotation.z + 1.0

-- Usando logs através do painel nativo C++
if transform.rotation.z > 360.0 then
    transform.rotation.z = 0.0
    Vamos.Log.Warn("Um loop de rotação completo da malha no Lua!")
    
    -- Disparar a mecânica visual de partículas de Nier!
    Vamos.Particles.Play("magia_avancada", transform.position.x, transform.position.y, transform.position.z)
end

-- Vamos usar a VFS da Engine pra imprimir se existe algo
local exists = Vamos.FileSystem.Exists("VamosEngine")
if exists then
    -- Somente no primeiro frame logará isto
    -- Vamos.Log.Info("Arquivo VFS acessado via Lua!")
end
