CC = clang++
LINK = clang++
CFLAGS = -Wall -std=c++11 -stdlib=libc++ -g -fno-exceptions -O2
LDFLAGS =
CPPFLAGS =
SOURCE_DIR = src
BUILD_DIR = build
SOURCES = $(shell find $(SOURCE_DIR) -type f -name *.cpp)
OBJS = $(patsubst $(SOURCE_DIR)/%,$(BUILD_DIR)/%,$(SOURCES:.cpp=.o))
EXEC_NAME = loop
EXEC = $(BUILD_DIR)/$(EXEC_NAME)

all: $(EXEC)

run: all
	@$(EXEC)

-include $(OBJS:.o=.dep)

$(EXEC) : $(OBJS) $(VENDOR_OBJS)
	$(LINK) $(LDFLAGS) $(OBJS) $(VENDOR_OBJS) $(LIB_DEP_FLAGS) $(FRAMEWORK_FLAGS) -o $@

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@
	@$(CC) $(CFLAGS) $(CPPFLAGS) -MM $(SOURCE_DIR)/$*.cpp > $(BUILD_DIR)/$*.dep
	@cp -f $(BUILD_DIR)/$*.dep $(BUILD_DIR)/$*.dep.tmp
	@sed -e 's|.*:|$(BUILD_DIR)/$*.o:|' < $(BUILD_DIR)/$*.dep.tmp > $(BUILD_DIR)/$*.dep
	@sed -e 's/.*://' -e 's/\\$$//' < $(BUILD_DIR)/$*.dep.tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(BUILD_DIR)/$*.dep
	@rm -f $(BUILD_DIR)/$*.dep.tmp

clean-all: clean test-clean vendor-clean

clean:
	rm -rf $(BUILD_DIR)

include vendor.make test.make