#pragma once

#include "../../vendor/sdl/include/SDL.h"

struct LfNode {
	LfNode* next;

	int length() const {
		const LfNode* cur = this;
		int len = 0;
		while (cur) {
			len++;
			cur = cur->next;
		}
		return len;
	}

	LfNode* tail() {
		LfNode* cur = this;
		while (cur->next) cur = cur->next;
		return cur;
	}

	LfNode* reverse() {
		LfNode* todo = this;
		LfNode* done = nullptr;

		while (todo) {
			LfNode* n = todo;
			todo = todo->next;
			n->next = done;
			done = n;
		}

		return done;
	}
};

struct LfStack {
	LfNode* head;

	LfStack() {
		head = nullptr;
	}

	bool push(LfNode* n) {
		LfNode* prevHead;
		do {
			prevHead = this->head;
			n->next = prevHead;
		} while (SDL_AtomicCASPtr((void**)&this->head, prevHead, n) == SDL_FALSE);
		return prevHead == nullptr;
	}

	LfNode* popAll() {
		return (LfNode*)SDL_AtomicSetPtr((void**)&this->head, nullptr);
	}

	bool isEmpty() const {
		return this->head == nullptr;
	}
};