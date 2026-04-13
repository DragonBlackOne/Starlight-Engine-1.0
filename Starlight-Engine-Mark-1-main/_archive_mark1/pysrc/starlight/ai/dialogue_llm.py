"""Integration hooks for LLM-based NPC Dialogue.

Supports local LLM inference via Ollama API with streaming,
graceful offline fallback, and conversation history.

Usage:
    agent = LLMDialogueAgent(model="llama3.2")
    response = agent.generate_response("Hello, guard!", context={"mood": "suspicious"})
"""
from __future__ import annotations
from typing import Any, Callable
import json


class LLMDialogueAgent:
    """Wrapper for LLM-based NPC dialogue with Ollama backend."""

    def __init__(
        self,
        model: str = "llama3.2",
        url: str = "http://localhost:11434/api/generate",
        system_prompt: str | None = None,
        max_history: int = 20,
    ) -> None:
        self.model = model
        self.url = url
        self.system_prompt = system_prompt or (
            "You are an NPC in a fantasy game world. "
            "Respond in character, briefly and naturally. "
            "Do not break character or mention being an AI."
        )
        self.history: list[dict[str, str]] = []
        self.max_history = max_history
        self._online: bool | None = None  # None = not yet checked

    def generate_response(
        self,
        user_text: str,
        context: dict[str, Any] | None = None,
        stream: bool = False,
    ) -> str:
        """Generate a response from the LLM.

        Args:
            user_text: What the player said.
            context: Optional dict with NPC state (mood, role, location, etc.)
            stream: If True, print tokens as they arrive (blocking).

        Returns:
            The NPC's response text.
        """
        # Build prompt with context
        full_system = self.system_prompt
        if context:
            ctx_str = ", ".join(f"{k}: {v}" for k, v in context.items())
            full_system += f"\nCurrent context: {ctx_str}"

        # Build conversation
        prompt_parts = [f"System: {full_system}"]
        for msg in self.history[-self.max_history:]:
            role = msg["role"].capitalize()
            prompt_parts.append(f"{role}: {msg['content']}")
        prompt_parts.append(f"User: {user_text}")
        prompt_parts.append("Assistant:")

        full_prompt = "\n".join(prompt_parts)

        # Try HTTP request
        try:
            import requests
            payload = {
                "model": self.model,
                "prompt": full_prompt,
                "stream": stream,
            }

            if stream:
                return self._stream_response(payload, user_text)

            resp = requests.post(self.url, json=payload, timeout=30)
            resp.raise_for_status()
            data = resp.json()
            response_text = data.get("response", "").strip()

            if response_text:
                self._online = True
                self.add_memory(user_text, response_text)
                return response_text

        except Exception as e:
            self._online = False
            print(f"[LLM] Offline/Error: {e}")

        # Fallback: simple rule-based responses
        return self._fallback_response(user_text, context)

    def _stream_response(self, payload: dict, user_text: str) -> str:
        """Stream tokens from Ollama API."""
        import requests
        collected = []
        try:
            with requests.post(self.url, json=payload, stream=True, timeout=30) as resp:
                resp.raise_for_status()
                for line in resp.iter_lines():
                    if line:
                        data = json.loads(line)
                        token = data.get("response", "")
                        collected.append(token)
                        print(token, end="", flush=True)
                        if data.get("done", False):
                            break
            print()  # newline after streaming
        except Exception as e:
            print(f"\n[LLM] Stream error: {e}")
            return self._fallback_response(user_text, None)

        response_text = "".join(collected).strip()
        if response_text:
            self.add_memory(user_text, response_text)
        return response_text

    def _fallback_response(self, user_text: str, context: dict[str, Any] | None) -> str:
        """Offline fallback with simple pattern matching."""
        text_lower = user_text.lower()
        role = (context or {}).get("role", "npc")

        if any(w in text_lower for w in ["hello", "hi", "hey", "greetings"]):
            responses = {
                "guard": "Move along, citizen.",
                "merchant": "Welcome! Browse my wares.",
                "blacksmith": "Need something forged?",
            }
            return responses.get(role, "Greetings, traveler.")

        if any(w in text_lower for w in ["quest", "mission", "task"]):
            return "I may have something for you... speak with the elder."

        if any(w in text_lower for w in ["buy", "sell", "trade", "shop"]):
            return "Let me show you what I have."

        if any(w in text_lower for w in ["bye", "farewell", "goodbye"]):
            return "Safe travels."

        return "I have nothing more to say."

    @property
    def is_online(self) -> bool:
        """Whether the last LLM call succeeded."""
        return self._online is True

    def clear_history(self) -> None:
        self.history.clear()

    def add_memory(self, user: str, agent: str) -> None:
        self.history.append({"role": "user", "content": user})
        self.history.append({"role": "assistant", "content": agent})
        # Trim if too long
        if len(self.history) > self.max_history * 2:
            self.history = self.history[-self.max_history * 2:]
