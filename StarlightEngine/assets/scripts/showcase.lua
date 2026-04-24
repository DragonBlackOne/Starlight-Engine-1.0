-- ==========================================
-- Vamos Engine - Titan Showcase Sentinel Script
-- Final Cyber-Industrial Version
-- ==========================================

function Update(dt)
    -- 1. Efeito de Flutuação (Sine Wave)
    local t = transform
    local time = os.clock()
    
    -- Flutuação em Y
    t.position.y = 5.0 + math.sin(time * 2.0) * 0.5
    
    -- Rotação constante
    t.rotation.y = t.rotation.y + dt * 45.0
    
    -- 2. Pulsação de Energia (Material)
    -- local mat = registry:get_material(entity_id) -- Assumindo que temos acesso ao registry
    -- Por enquanto, vamos simular a pulsação via Log ou se tivermos API direta
    -- O C++ cuida de ler o transform, mas a cor precisa da API de Registro.
    
    -- Se estiver muito perto do centro (0,0,0), emitir alerta
    local dist = math.sqrt(t.position.x^2 + t.position.z^2)
    if dist < 2.0 then
        Vamos.Log.Warn("Sentinela muito próxima do Núcleo Titan!")
        Vamos.Particles.Play("alerta_sentinela", t.position.x, t.position.y, t.position.z)
    end
end
