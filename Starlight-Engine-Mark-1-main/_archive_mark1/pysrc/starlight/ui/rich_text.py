"""Rich Text Parser & Renderer.

Usage:
    rt = RichTextParser()
    tokens = rt.parse("Hello <red>World</red>!")
"""
from __future__ import annotations
from dataclasses import dataclass

@dataclass
class TextSegment:
    text: str
    color: tuple[float,float,float,float] = (1,1,1,1)
    bold: bool = False
    italic: bool = False
    size_mult: float = 1.0

class RichTextParser:
    """Parses markdown-like or XML-like tags."""
    
    def parse(self, text: str) -> list[TextSegment]:
        segments = []
        # Simple mock parser
        # Real one would use regex or a state machine
        if "<red>" in text:
            parts = text.split("<red>")
            segments.append(TextSegment(parts[0]))
            
            rest = parts[1]
            if "</red>" in rest:
                inner, outer = rest.split("</red>")
                segments.append(TextSegment(inner, color=(1,0,0,1)))
                segments.append(TextSegment(outer))
            else:
                segments.append(TextSegment(rest))
        else:
             segments.append(TextSegment(text))
             
        return segments
