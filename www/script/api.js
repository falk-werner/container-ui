

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

    async #post(url) {
        const response = await fetch(url, {
            method: "POST",
            headers: {
                "Authorization": `Bearer ${this.#access_token}`
            }
        });
        if ((response.status < 200) || (300 <= response.status)) {
            throw new Error();
        }
    }

    async #remove(url) {
        const response = await fetch(url, {
            method: "DELETE",
            headers: {
                "Authorization": `Bearer ${this.#access_token}`
            }
        });
        if ((response.status < 200) || (300 <= response.status)) {
            throw new Error();
        }
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
        return await this.#fetch_json("api/containers/json?all=true&size=true");
    }

    async container_inspect(id) {
        return await this.#fetch_json(`api/containers/${id}/json?size=true`);
    }

    async container_stats(id) {
        return await this.#fetch_json(`api/containers/${id}/stats?one-shot=true&stream=false`);
    }

    async container_start(id)  {
        await this.#post(`api/containers/${id}/start`);
    }

    async container_stop(id)  {
        await this.#post(`api/containers/${id}/stop`);
    }

    async container_pause(id)  {
        await this.#post(`api/containers/${id}/pause`);
    }

    async container_unpause(id)  {
        await this.#post(`api/containers/${id}/unpause`);
    }

    async container_restart(id)  {
        await this.#post(`api/containers/${id}/restart`);
    }

    async container_kill(id)  {
        await this.#post(`api/containers/${id}/kill`);
    }

    async container_remove(id)  {
        await this.#remove(`api/containers/${id}`);
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
        await this.#post("api/volumes/prune")
    }

    async volume_remove(name)  {
        await this.#remove(`api/volumes/${name}`);
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
