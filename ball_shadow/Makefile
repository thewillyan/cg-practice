CXX = clang++
CXXFLAGS = -Weverything  -Wno-global-constructors

main: ball_shadow.cpp
	$(CXX) $(CXXFLAGS) -lGL -lglut ./ball_shadow.cpp -o ball_shadow

run: main
	./ball_shadow
