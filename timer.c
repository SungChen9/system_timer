

///////////////////////////////////////////////////////////////
//provide to user
typedef {
	int id;
	int expire;
	int interval;
	void (*function)(int);
	int parameter;
}timer_t;

int timer_create(timer_t *timer, int delay_ms, int interval, void (*function)(int) callback, int para);
int timer_settime(timer_t *timer);
int timer_delete(timer_t *timer);
//provide to user
///////////////////////////////////////////////////////////////


//provide from hardware, 
int hr_reset_timer(int expire);

int next_timer_id = 0;
multimap<int, timer_t*> waits;
queue<pair<void (*)(int), int>> todos;

int timer_create(timer_t *timer, int delay_ms, int interval, int para) {
	timer->id = next_timer_id;
	next_timer_id++;
	timer->expire = getcurtime() + delay;
	timer->interval = interval;
	timer->function = callback;
	timer->parameter = para;
	return 0;
}

int timer_settime(timer_t *timer) {
	int old_next_expire = INT_MAX;
	if (waits.size() > 0) {
		old_next_expire = waits.begin()->first;
	}
	waits.insert({timer->expire, timer});
	if (timer->expire < old_next_expire) {
		hr_reset_timer(timer->expire);
	}
}

int timer_delete(timer_t *timer) {
	if (waits.size() == 0 || timer == nullptr) {
		return -1;
	}
	const int old_next_expire = waits.begin()->first;
	pair<multimap<int, timer_t*>, multimap<int, timer_t*>> range = waits.equal_range(timer->expire);
	for (auto it = range->first; it != range->second; it++) {
		if (it.second == timer) {
			waits.erase(it);
			break;
		}
	}
	
	if (waits.size() == 0) {
		hr_reset_timer(0);
		return 0;
	}
	const int new_next_expire = waits.begin()->first;
	if (old_next_expire != new_next_expire) {
		hr_reset_timer(new_next_expire);
	}
	return 0;
}

void hw_timer_isr() {
	const int curtime = getcurtime();

	while (waits.size() > 0 && waits.begin()->first <= curtime) {
		timer_t *timer = waits.begin()->second;
		waits.erase(waits.begin());
		todos.push({timer->function, timer->para});
		if (timer->interval) {
			timer->expire = curtime + timer->interval;
			waits.insert({timer->expire, timer});
		}
	}

	if (waits.size() > 0) {
		hr_reset_timer(waits.begin()->first);
	}
}



//timer service task
void timer_task() {
	while (1) {
		while (todos.size() > 0) {
			pthread_create(todos.front().first, todos.front().second);
			todos.pop();
		}
	}
}
