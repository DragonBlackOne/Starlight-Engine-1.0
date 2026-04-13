import glm


def test_glm_sanity():
    v = glm.vec2(1, 0)
    assert glm.length(v) == 1.0
