import { sha256, b64url_encode, generate_token } from "./util.js";

const client_id = "container_ui";
const scope = "container_ui";
const redirect_uri = "/";

async function do_login() {
    const code_verifier = generate_token();
    localStorage.setItem("code_verifier", code_verifier);

    const code_challenge = b64url_encode(await sha256(code_verifier));

    const url = "/auth/authorize?" +
        "response_type=code&" +
        `client_id=${client_id}&` +
        `redirect_uri=${redirect_uri}&` +
        `scope=${scope}&` +
        "state=init&" +
        "code_challenge_method=S256&" +
        `code_challenge=${code_challenge}`;
    
    window.location.href = url;
}

async function get_access_token(code, code_verifier) {
    const body = "grant_type=authorization_code&" +
        `code=${code}&` +
        `client_id=${client_id}&` +
        `code_verifier=${code_verifier}&` +
        `redirect_uri=${redirect_uri}`;

    const resp = await fetch("/auth/token", {
        "method": "POST",
        "headers": {
            "Content-Type": "application/x-www-form-urlencoded"
        },
        "body": body
    });

    const data = await resp.json();
    return data.access_token;
}

export async function login() {
    const query_args = new URLSearchParams(window.location.search);
    if (query_args.size == 0) {
        do_login();
        return null;
    }

    try {
        const code = query_args.get("code");
        const code_verifier = localStorage.getItem("code_verifier");
        localStorage.removeItem("code_verifier");
        history.pushState({}, "", "/");

        if ((code) && (code_verifier)) {
            const access_token = await get_access_token(code, code_verifier);
            return access_token;
        }
    }
    catch (ex) {
        // swallow
    } 

    return null;
}