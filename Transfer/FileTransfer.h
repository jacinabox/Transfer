#ifndef _FILES
#define _FILES

#include <stdio.h>
#include <utility>
#include <functional>

using namespace std::placeholders;

std::pair<unsigned, char> reader_helper(FILE* handle, const std::pair<unsigned, char>& position, const Nothing& _dummy_input) {
	fseek(handle, position.first, SEEK_SET);
	char c = fgetc(handle);

	return std::make_pair(1 + position.first, c);
}

template<class I> Nothing writer_helper(FILE* handle, const std::pair<unsigned, char>& pair) {
	fseek(handle, pair.first, SEEK_SET);
	fputc(handle, pair.second);
	return 0;
}

template<class T, class U> T first_(const std::pair<T, U>& pair) {
	return pair.first;
}

template<class T, class U> U second_(const std::pair<T, U>& pair){
	return pair.second;
}

template<class I> Transfer<Nothing, char>& reader(FILE* handle) {
	std::function<std::pair<unsigned, char>(const std::pair<unsigned, char>&, const Nothing&)> f(
		std::bind(reader_helper, handle, _1, _2));
	std::pair<unsigned, char> init(std::make_pair(0, '\0'));

	return scanning(init, f) >>
		map(second_<unsigned, char>);
}

template<class I> Transfer<I, std::pair<unsigned, char> >& reader2(const I& type_determiner, FILE* handle) {
	std::function<std::pair<unsigned, char>(const std::pair<unsigned, char>&, const I&)> f(
		std::bind(reader_helper<I>, handle, _1, _2));
	std::pair<unsigned, char> init(std::make_pair(0, '\0'));

	return scanning(init, f);
}

template<class I> Transfer<std::pair<unsigned, char>, Nothing>& writer(FILE* handle) {
	std::function < std::pair<unsigned, char>(const std::pair<unsigned, char>&)> f(
		std::bind(writer_helper, handle)
	);
	std::pair<unsigned, char> init(std::make_pair(0, '\0'));

	return map(f);
}

#endif