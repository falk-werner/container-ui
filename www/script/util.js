export async function sha256(text) {
    const encoder = new TextEncoder();
    const data = encoder.encode(text);
    const hash = await window.crypto.subtle.digest("SHA-256", data);
    return new Uint8Array(hash);
}

export function b64url_encode(data) {
    let encoded = data.toBase64();
    encoded = encoded.replaceAll("+", "-");
    encoded = encoded.replaceAll("/", "_");
    encoded = encoded.replaceAll("=", "");
    return encoded;
}

export function generate_token() {
    const data = new Uint8Array(42);
    window.crypto.getRandomValues(data);

    return b64url_encode(data);
}

export function format_mem(value) {
    if (value < 1024) {
        return `${value} Bytes`;
    }
    value /= 1024;

    if (value < 1024) {
        return `${value.toFixed(1)} KBytes`;
    }
    value /= 1024;
    
    if (value < 1024) {
        return `${value.toFixed(1)} MBytes`;
    }
    value /= 1024;

    return `${value.toFixed(1)} GBytes`;
}

export function set_text(id, value) {
    const element = document.querySelector(id);
    element.textContent = value;

}
