

export class Api {

    #access_token

    constructor(access_token) {
        this.#access_token = access_token;
    }

    async #fetch_json(url) {
        const response = await fetch(url, {
            headers: {
                "Authorization": `Bearer ${this.#access_token}`
            }
        });
        return await response.json();
    }

    async version() {
        return await this.#fetch_json("api/version");
    }

    async info() {
        return await this.#fetch_json("api/info");
    }

    async disk_usage() {
        return await this.#fetch_json("api/system/df");
    }

    async containers() {
        return await this.#fetch_json("api/containers/json?all=true");
    }

    async images() {
        return await this.#fetch_json("api/images/json?all=true");
    }

    async volumes() {
        return await this.#fetch_json("api/volumes");
    }
}
