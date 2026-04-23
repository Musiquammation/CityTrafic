import os
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler

class Translation:
	def __init__(self, name, NAME, srcFile, hppFile, tsFile):
		self.name = name
		self.NAME = NAME
		self.srcFile = os.path.abspath(srcFile)
		self.hppFile = os.path.abspath(hppFile)
		self.tsFile = os.path.abspath(tsFile)

def translation(name, NAME):
	# Generates the object with the requested relative paths
	return Translation(
		name, 
		NAME,
		f"def/{NAME}.def", 
		f"server/{name}.hpp", 
		f"client/src/shared/{name}.ts"
	)

# --- File configuration ---
translations = [
	translation("ServerId", "SERVER_IDS"),
	translation("ClientId", "CLIENT_IDS"),
	Translation(
		"CommandCode",
		"COMMAND_CODES",
		"def/COMMAND_CODES.def", 
		"game/CommandCode.hpp", 
		"client/src/shared/CommandCode.ts"
	)
]

def generate_files(t: Translation):
	if not os.path.exists(t.srcFile):
		print(f"⚠️  Source file not found: {t.srcFile}")
		return

	print(f"🔄 Generating for {t.name}...")
	
	with open(t.srcFile, 'r') as f:
		lines = f.readlines()

	# Parse: one identifier per line, ignore empty lines / comments
	ids = []
	for line in lines:
		line = line.strip()
		if not line or line.startswith("#"):
			continue
		ids.append(line)

	if not ids:
		print(f"⚠️  No identifiers found in {t.srcFile}")
		return

	# ---- C++ generation ----
	enum_name = t.name[0].upper() + t.name[1:]

	cpp_content = "// Auto-generated file\n#pragma once\n\n\n"
	cpp_content += f"enum class {enum_name} {{\n"
	for name in ids:
		cpp_content += f"\t{name},\n"
	cpp_content += "};\n"

	os.makedirs(os.path.dirname(t.hppFile), exist_ok=True)
	with open(t.hppFile, 'w') as f:
		f.write(cpp_content)

	# ---- TypeScript generation ----
	ts_content = "// Auto-generated file\n\n"
	ts_content += f"export enum {t.NAME} {{\n"
	for name in ids:
		ts_content += f"\t{name},\n"
	ts_content += "}\n"

	os.makedirs(os.path.dirname(t.tsFile), exist_ok=True)
	with open(t.tsFile, 'w') as f:
		f.write(ts_content)

	print(f"✅ Done: {t.hppFile} & {t.tsFile}")


class DefHandler(FileSystemEventHandler):
	def on_modified(self, event):
		if event.is_directory:
			return
		for t in translations:
			# If the modified file matches a source
			if os.path.abspath(event.src_path) == t.srcFile:
				generate_files(t)

if __name__ == "__main__":
	# Initial run at startup
	for t in translations:
		generate_files(t)

	# Setup file watcher
	observer = Observer()
	# Watch the parent directory of the sources (def)
	commons_dir = os.path.abspath("def")
	observer.schedule(DefHandler(), commons_dir, recursive=False)
	
	print(f"👀 Watching for changes in: {commons_dir}")
	observer.start()
	try:
		observer.join()
	except KeyboardInterrupt:
		observer.stop()