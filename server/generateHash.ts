export function generateHash(): string {
	const bytes = new Uint8Array(8);
	crypto.getRandomValues(bytes);
	let hex = '';
	for (const b of bytes) {
		hex += ((b >> 4) & 0xF).toString(16);
		hex += (b & 0xF).toString(16);
	}
	return hex;
}