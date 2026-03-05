

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

    async container_inspect(id) {
        return await this.#fetch_json(`api/containers/${id}/json?size=true`);
    }

    async container_stats(id) {
        return await this.#fetch_json(`api/containers/${id}/stats?one-shot=true&stream=false`);
    }

    async images() {
        return await this.#fetch_json("api/images/json?all=true");
    }

    async image_inspect(id)  {
        return await this.#fetch_json(`api/images/${id}/json`);
    }

    async volumes() {
        return await this.#fetch_json("api/volumes");
    }

    async volume_inspect(name)  {
        return await this.#fetch_json(`api/volumes/${name}`);
    }

    async volumes_prune()  {
        const response = await fetch("api/volumes/prune", {
            method: "POST",
            headers: {
                "Authorization": `Bearer ${this.#access_token}`
            }
        });
        if ((response.status < 200) || (300 <= response.status)) {
            throw new Error();
        }
    }

    async volume_remove(name)  {
        const response = await fetch(`api/volumes/${name}`, {
            method: "DELETE",
            headers: {
                "Authorization": `Bearer ${this.#access_token}`
            }
        });
        if ((response.status < 200) || (300 <= response.status)) {
            throw new Error();
        }
    }

    async volume_create(name) {
        const response = await fetch("api/volumes/create", {
            method: "POST",
            headers: {
                "Content-Type": "application/json",
                "Authorization": `Bearer ${this.#access_token}`
            },
            body: JSON.stringify({
                "Driver": "local",
                "DriverOpts": {},
                "Labels": {},
                "Name": name
            })
        });

        if ((response.status < 200) || (300 <= response.status)) {
            throw new Error();
        }
    }
}
