TEST_CPPFLAGS = -DTEST
TEST_BUILD_DIR = testbuild
TEST_OBJS = $(patsubst $(SOURCE_DIR)/%,$(TEST_BUILD_DIR)/%,$(SOURCES:.cpp=.o))
TEST_EXEC_NAME = loop_test
TEST_EXEC = $(TEST_BUILD_DIR)/$(TEST_EXEC_NAME)

test-debug:
	echo $(SOURCE_DIR)
	echo $(TEST_OBJS)

test: $(TEST_EXEC)

test-run: test
	@$(TEST_EXEC)

-include $(TEST_OBJS:.o=.dep)

$(TEST_EXEC): $(TEST_OBJS) $(VENDOR_OBJS)
	$(LINK) $(TEST_OBJS) $(VENDOR_OBJS) $(LIB_DEP_FLAGS) $(FRAMEWORK_FLAGS) -o $@

$(TEST_BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(TEST_CPPFLAGS) -c $< -o $@
	@$(CC) $(CFLAGS) $(TEST_CPPFLAGS) -MM $(SOURCE_DIR)/$*.cpp > $(TEST_BUILD_DIR)/$*.dep
	@cp -f $(TEST_BUILD_DIR)/$*.dep $(TEST_BUILD_DIR)/$*.dep.tmp
	@sed -e 's|.*:|$(TEST_BUILD_DIR)/$*.o:|' < $(TEST_BUILD_DIR)/$*.dep.tmp > $(TEST_BUILD_DIR)/$*.dep
	@sed -e 's/.*://' -e 's/\\$$//' < $(TEST_BUILD_DIR)/$*.dep.tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(TEST_BUILD_DIR)/$*.dep
	@rm -f $(TEST_BUILD_DIR)/$*.dep.tmp

test-clean:
	rm -rf $(TEST_BUILD_DIR)